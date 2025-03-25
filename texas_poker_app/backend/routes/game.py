from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Depends, HTTPException
from ..models.game import PokerGame, Player
from ..routes.auth import get_current_user
from ..models.database import get_db
import json
from starlette.websockets import WebSocketState  # Добавляем импорт

router = APIRouter()

class ConnectionManager:
    def __init__(self):
        self.active_connections = {}  # {websocket: {"player_id": str, "ready": bool}}
        self.game = None
        self.players = {}  # {player_id: Player}
        self.last_dealer_index = -1  # Для отслеживания последнего дилера
        self.waiting_players = set()  # Игроки, которые ждут окончания текущего раунда

    async def connect(self, websocket: WebSocket, player_id: str):
        await websocket.accept()
        if player_id not in self.players:
            self.players[player_id] = Player(player_id)
        self.active_connections[websocket] = {"player_id": player_id, "ready": False}
        print(f"Player {player_id} connected")
        if self.game and self.game.stage:  # Если игра уже идёт
            self.waiting_players.add(player_id)
            await websocket.send_text(json.dumps({"message": "Waiting for the current round to finish"}))
        await self.broadcast_state()

    async def disconnect(self, websocket: WebSocket):
        if websocket in self.active_connections:
            player_id = self.active_connections[websocket]["player_id"]
            print(f"Player {player_id} disconnected")
            del self.active_connections[websocket]

            if not self.active_connections:
                self.game = None
                self.last_dealer_index = -1
                self.waiting_players.clear()
                print("No active connections, game reset")
                return

            if self.game:
                player_ids = [p.name for p in self.game.players]
                if player_id in player_ids:
                    player_index = player_ids.index(player_id)
                    self.game.players[player_index].fold()
                    print(f"Player {player_id} folded due to disconnect")

                    players_in_game = [p for p in self.game.players if not p.folded]
                    active_players = [p for p in self.game.players if not p.folded and p.balance > 0]

                    if len(players_in_game) <= 1:
                        print("Only one player left in game, ending round")
                        while self.game.stage != "showdown" and self.game.advance_stage():
                            new_game_state = self.game.get_game_state(0)
                            print(f"Automatically advancing to next stage: {new_game_state['stage']}")
                        winner = self.game.get_winner()
                        await self.broadcast_winner(winner, True)
                        return

                    next_player_index = self.game.next_active_player(self.game.current_player_index)
                    if next_player_index == -1:
                        print("No active players left after disconnect, ending round")
                        while self.game.stage != "showdown" and self.game.advance_stage():
                            new_game_state = self.game.get_game_state(0)
                            print(f"Automatically advancing to next stage: {new_game_state['stage']}")
                        winner = self.game.get_winner()
                        await self.broadcast_winner(winner, True)
                        return
                    self.game.current_player_index = next_player_index
                    await self.check_and_advance_stage()

            # Убираем вызов broadcast_state, так как broadcast_game_state уже отправляет состояние
            # await self.broadcast_state()

    async def broadcast_state(self):
        if not self.active_connections:
            return
        num_players = len(self.active_connections)
        state = {
            "players": [
                {
                    "player_id": data["player_id"],
                    "ready": data["ready"] if not self.game else None,  # Отправляем ready только если игра не активна
                    "balance": self.players[data["player_id"]].balance,
                    "is_dealer": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 1) % num_players,
                    "is_small_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 2) % num_players,
                    "is_big_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 3) % num_players,
                }
                for idx, data in enumerate(self.active_connections.values())
            ],
            "game_active": bool(self.game)
        }
        print(f"Broadcasting state: {state}")
        all_ready = all(data["ready"] for data in self.active_connections.values())
        if all_ready and len(self.active_connections) > 1 and not self.game:
            await self.start_game()
        else:
            disconnected = []
            for websocket in list(self.active_connections.keys()):
                try:
                    if websocket.client_state == WebSocketState.CONNECTED:
                        await websocket.send_text(json.dumps(state))
                    else:
                        print(
                            f"Skipping send to {self.active_connections[websocket]['player_id']}: WebSocket not connected")
                        disconnected.append(websocket)
                except (WebSocketDisconnect, RuntimeError) as e:
                    print(f"Error sending state to {self.active_connections[websocket]['player_id']}: {e}")
                    disconnected.append(websocket)
            for ws in disconnected:
                await self.disconnect(ws)

    async def start_game(self):
        player_ids = [data["player_id"] for data in self.active_connections.values()]
        active_player_ids = [pid for pid in player_ids if
                             self.players[pid].balance > 0 and pid not in self.waiting_players]
        print(f"Player IDs in start_game: {player_ids}")
        print(f"Active player IDs after filtering: {active_player_ids}")

        if len(active_player_ids) < 2:
            print("Not enough players with balance to start the game")
            disconnected = []
            for websocket in list(self.active_connections.keys()):
                try:
                    if websocket.client_state == WebSocketState.CONNECTED:
                        await websocket.send_text(
                            json.dumps({"error": "Not enough players with balance to start the game"}))
                    else:
                        disconnected.append(websocket)
                except (WebSocketDisconnect, RuntimeError) as e:
                    print(f"Error sending error message to {self.active_connections[websocket]['player_id']}: {e}")
                    disconnected.append(websocket)
            for ws in disconnected:
                await self.disconnect(ws)
            self.game = None
            return

        if self.game:
            self.last_dealer_index = self.game.dealer_index
        print(f"Last dealer_index before new game: {self.last_dealer_index}")

        self.game = PokerGame([])
        self.game.players = [self.players[player_id] for player_id in active_player_ids]

        if self.last_dealer_index >= 0:
            self.game.dealer_index = (self.last_dealer_index + 1) % len(self.game.players)
        else:
            self.game.dealer_index = 0

        try:
            self.game.start_new_round()
        except ValueError as e:
            print(f"Error starting new round: {e}")
            disconnected = []
            for websocket in list(self.active_connections.keys()):
                try:
                    if websocket.client_state == WebSocketState.CONNECTED:
                        await websocket.send_text(json.dumps({"error": str(e)}))
                    else:
                        disconnected.append(websocket)
                except (WebSocketDisconnect, RuntimeError) as e:
                    print(f"Error sending error message to {self.active_connections[websocket]['player_id']}: {e}")
                    disconnected.append(websocket)
            for ws in disconnected:
                await self.disconnect(ws)
            self.game = None
            return

        print("Game started, sending initial state")
        disconnected = []
        for websocket, data in list(self.active_connections.items()):
            try:
                if websocket.client_state == WebSocketState.CONNECTED:
                    if data["player_id"] in active_player_ids:
                        player_index = active_player_ids.index(data["player_id"])
                        game_state = self.game.get_game_state(player_index)
                        # Убедимся, что ready не отправляется в активной игре
                        for player in game_state["players"]:
                            player["ready"] = None  # Не отправляем ready во время игры
                        print(f"Sending to {data['player_id']}: {game_state}")
                        await websocket.send_text(json.dumps(game_state))
                    else:
                        await websocket.send_text(json.dumps(
                            {"error": "You cannot participate because your balance is 0 or you are waiting"}))
                else:
                    disconnected.append(websocket)
            except (WebSocketDisconnect, RuntimeError) as e:
                print(f"Error sending initial state to {data['player_id']}: {e}")
                disconnected.append(websocket)
        for ws in disconnected:
            await self.disconnect(ws)

    async def set_ready(self, websocket: WebSocket, ready: bool):
        if websocket in self.active_connections:
            self.active_connections[websocket]["ready"] = ready
            await self.broadcast_state()

    async def handle_action(self, websocket: WebSocket, action: str, value=None):
        if not self.game:
            await websocket.send_text(json.dumps({"error": "Game not started, please set ready"}))
            return

        player_id = self.active_connections[websocket]["player_id"]
        if player_id in self.waiting_players:
            await websocket.send_text(json.dumps({"error": "You are waiting for the current round to finish"}))
            return

        if player_id not in [p.name for p in self.game.players]:
            await websocket.send_text(json.dumps({"error": "You are not in the current game"}))
            return

        player_index = [p.name for p in self.game.players].index(player_id)
        game_state = self.game.get_game_state(player_index)
        print(
            f"Handling action for player {player_id} (index {player_index}), current player: {game_state['current_player']}"
        )

        if game_state["current_player"] != player_id:
            await websocket.send_text(json.dumps({"error": "Not your turn"}))
            await self.broadcast_game_state()
            return

        try:
            if action == "bet":
                if not self.game.place_bet(player_index, value):
                    await websocket.send_text(json.dumps({"error": "Invalid bet"}))
                    return
            elif action == "call_or_pass":
                if not self.game.call_or_pass(player_index):
                    await websocket.send_text(json.dumps({"error": "Cannot call or pass"}))
                    return
            elif action == "fold":
                if not self.game.fold(player_index):
                    await websocket.send_text(json.dumps({"error": "Cannot fold"}))
                    return
            elif action == "all_in":
                if not self.game.all_in(player_index):
                    await websocket.send_text(json.dumps({"error": "Cannot go all-in"}))
                    return

            previous_stage = self.game.stage
            while True:
                await self.check_and_advance_stage()
                current_stage = self.game.stage
                if current_stage == previous_stage:
                    break
                previous_stage = current_stage

        except ValueError as e:
            await websocket.send_text(json.dumps({"error": str(e)}))
            await self.broadcast_game_state()

    async def handle_player_ready(self, websocket: WebSocket):
        player_id = self.active_connections[websocket]["player_id"]
        if player_id in self.waiting_players:
            await websocket.send_text(json.dumps({"error": "You are waiting for the current round to finish"}))
            return

        if not self.game:
            # Если игра не активна, устанавливаем ready и начинаем новую игру, если все готовы
            await self.set_ready(websocket, True)
            return

        if self.game.stage != "showdown":
            await websocket.send_text(json.dumps({"error": "Cannot set ready during an active game"}))
            return

        # Устанавливаем ready для игрока
        self.active_connections[websocket]["ready"] = True
        all_ready = all(data["ready"] for data in self.active_connections.values())
        if all_ready:
            self.last_dealer_index = self.game.dealer_index
            self.game = None
            state = {
                "all_ready": True,
                "players": [
                    {
                        "player_id": data["player_id"],
                        "ready": False,
                        "balance": self.players[data["player_id"]].balance,
                        "is_dealer": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 1) % len(
                            self.active_connections),
                        "is_small_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 2) % len(
                            self.active_connections),
                        "is_big_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 3) % len(
                            self.active_connections),
                    }
                    for idx, data in enumerate(self.active_connections.values())
                ]
            }
            print(f"All ready state: {state}")
            disconnected = []
            for ws in list(self.active_connections.keys()):
                try:
                    if ws.client_state == WebSocketState.CONNECTED:
                        await ws.send_text(json.dumps(state))
                    else:
                        disconnected.append(ws)
                except (WebSocketDisconnect, RuntimeError) as e:
                    print(f"Error sending all_ready state to {self.active_connections[ws]['player_id']}: {e}")
                    disconnected.append(ws)
            for ws in self.active_connections:
                self.active_connections[ws]["ready"] = False
            for ws in disconnected:
                await self.disconnect(ws)
            await self.start_game()
        else:
            # Отправляем обновлённое состояние игры
            disconnected = []
            for ws in list(self.active_connections.keys()):
                player_id = self.active_connections[ws]["player_id"]
                if player_id not in [p.name for p in self.game.players]:
                    continue
                player_index = [p.name for p in self.game.players].index(player_id)
                game_state = self.game.get_game_state(player_index)
                for i, player in enumerate(game_state["players"]):
                    player["balance"] = self.players[player["name"]].balance
                    player["ready"] = list(self.active_connections.values())[i]["ready"]
                try:
                    if ws.client_state == WebSocketState.CONNECTED:
                        await ws.send_text(json.dumps(game_state))
                    else:
                        disconnected.append(ws)
                except (WebSocketDisconnect, RuntimeError) as e:
                    print(f"Error sending ready state to {self.active_connections[ws]['player_id']}: {e}")
                    disconnected.append(ws)
            for ws in disconnected:
                await self.disconnect(ws)

    async def check_and_advance_stage(self):
        if not self.game:
            return
        game_state = self.game.get_game_state(0)
        active_players = [p for p in game_state["players"] if not p["folded"] and self.players[p["name"]].balance > 0]
        players_in_game = [p for p in game_state["players"] if not p["folded"]]
        all_folded = len(players_in_game) <= 1 and any(p["folded"] for p in game_state["players"])

        print(
            f"Checking stage: {self.game.stage}, active_players: {len(active_players)}, players_in_game: {len(players_in_game)}")

        # Завершаем игру, только если в игре остаётся 0 или 1 игрок, не в фолде
        if len(players_in_game) <= 1:
            while self.game.stage != "showdown" and self.game.advance_stage():
                new_game_state = self.game.get_game_state(0)
                print(f"Automatically advancing to next stage: {new_game_state['stage']}")
            winner = self.game.get_winner()
            print(f"Winner determined: {winner}")
            await self.broadcast_winner(winner, all_folded)
            return

        # Проверяем, если у всех игроков, кроме одного, закончились фишки
        if len(active_players) <= 1:
            max_bet = max(p["bet"] for p in players_in_game)
            all_bets_settled = all(p["bet"] == max_bet or self.players[p["name"]].balance == 0 for p in players_in_game)
            if all_bets_settled:
                while self.game.stage != "showdown" and self.game.advance_stage():
                    new_game_state = self.game.get_game_state(0)
                    print(f"Automatically advancing to next stage: {new_game_state['stage']}")
                winner = self.game.get_winner()
                print(f"Winner determined: {winner}")
                await self.broadcast_winner(winner, all_folded)
                return
            await self.broadcast_game_state()
            return

        # Если стадия может быть продвинута (все ставки уравнены и раунд завершён)
        if self.game.can_advance_stage():
            advanced = self.game.advance_stage()
            new_game_state = self.game.get_game_state(0)
            if advanced:
                print(f"Stage advanced to: {new_game_state['stage']}")
            if new_game_state["stage"] == "showdown":
                winner = self.game.get_winner()
                print(f"Winner determined: {winner}")
                await self.broadcast_winner(winner, all_folded)
                return
            if not advanced:
                print("Cannot advance stage after advance_stage call, broadcasting current state")
            await self.broadcast_game_state()
        else:
            print("Cannot advance stage, broadcasting current state")
            await self.broadcast_game_state()

    async def broadcast_game_state(self):
        if not self.game:
            return
        disconnected = []
        for websocket in list(self.active_connections.keys()):
            player_id = self.active_connections[websocket]["player_id"]
            if player_id in self.waiting_players:
                continue
            if player_id not in [p.name for p in self.game.players]:
                continue
            try:
                if websocket.client_state == WebSocketState.CONNECTED:
                    player_index = [p.name for p in self.game.players].index(player_id)
                    game_state = self.game.get_game_state(player_index)
                    for player in game_state["players"]:
                        player["balance"] = self.players[player["name"]].balance
                        if self.game.stage == "showdown":
                            player["ready"] = self.active_connections[websocket]["ready"]
                        else:
                            player["ready"] = None  # Не отправляем ready, если игра активна
                    print(f"Sending game state to {player_id}: {game_state}")
                    await websocket.send_text(json.dumps(game_state))
                else:
                    disconnected.append(websocket)
            except (WebSocketDisconnect, RuntimeError) as e:
                print(f"Error sending game state to {player_id}: {e}")
                disconnected.append(websocket)
        for ws in disconnected:
            await self.disconnect(ws)

    async def broadcast_winner(self, winner_data, all_folded):
        if not winner_data:  # Проверяем, что winner_data не None
            print("No winner determined, broadcasting current state")
            await self.broadcast_state()
            return

        state = {
            "stage": "showdown",  # Явно указываем стадию
            "winner": winner_data,
            "all_folded": all_folded,
            "players": [
                {
                    "name": p.name,
                    "folded": p.folded,
                    "bet": p.current_bet,
                    "total_bet": p.total_bet,
                    "balance": self.players[p.name].balance,
                    "hand": [str(card) for card in p.hand] if (
                                self.game.stage == "showdown" and not p.folded) else None,
                    "is_dealer": i == self.game.dealer_index,
                    "is_small_blind": i == (self.game.dealer_index + 1) % len(self.game.players),
                    "is_big_blind": i == (self.game.dealer_index + 2) % len(self.game.players),
                    "ready": self.active_connections[ws]["ready"] if ws in self.active_connections else False
                }
                for i, (ws, p) in enumerate([
                    (next((ws for ws, data in self.active_connections.items() if data["player_id"] == p.name), None), p)
                    for p in self.game.players
                ])
            ],
            "community_cards": [str(card) for card in self.game.revealed_cards],
            "pots": [{"amount": pot["amount"], "eligible_players": pot["eligible_players"]} for pot in self.game.pots]
        }

        # Добавляем руку победителя в state["winner"]
        if isinstance(winner_data["player"], list):
            state["winner"]["hand"] = winner_data["hand"]
        else:
            winner_id = winner_data["player"]
            winner_player = next((p for p in self.game.players if p.name == winner_id), None)
            if winner_player:
                state["winner"]["hand"] = [str(card) for card in winner_player.hand]

        disconnected = []
        for websocket in list(self.active_connections.keys()):
            player_id = self.active_connections[websocket]["player_id"]
            if self.game and player_id not in [p.name for p in self.game.players]:
                continue
            try:
                if websocket.client_state == WebSocketState.CONNECTED:
                    await websocket.send_text(json.dumps(state))
                else:
                    disconnected.append(websocket)
            except (WebSocketDisconnect, RuntimeError) as e:
                print(f"Error sending winner to {self.active_connections[websocket]['player_id']}: {e}")
                disconnected.append(websocket)
        for ws in disconnected:
            await self.disconnect(ws)

        # Сбрасываем игру, но не отправляем broadcast_state
        if self.game:
            self.last_dealer_index = self.game.dealer_index
        self.game = None  # Сбрасываем игру
        self.waiting_players.clear()  # Очищаем ожидающих игроков
        for ws in self.active_connections:
            self.active_connections[ws]["ready"] = False  # Сбрасываем состояние готовности
        # Не вызываем broadcast_state здесь, чтобы клиент оставался в состоянии showdown

        # Сбрасываем игру после объявления победителя
        if self.game:
            self.last_dealer_index = self.game.dealer_index
        self.game = None  # Сбрасываем игру
        self.waiting_players.clear()  # Очищаем ожидающих игроков
        for ws in self.active_connections:
            self.active_connections[ws]["ready"] = False  # Сбрасываем состояние готовности
        await self.broadcast_state()  # Отправляем предигровое состояние всем клиентам

manager = ConnectionManager()

@router.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket, token: str):
    db = next(get_db())
    try:
        username = await get_current_user(token, db)
    except HTTPException:
        await websocket.close(code=1008)
        return
    finally:
        db.close()

    await manager.connect(websocket, username)
    try:
        while True:
            data = await websocket.receive_text()
            message = json.loads(data)
            action = message.get("action")
            value = message.get("value")
            if action == "ready":
                if manager.game:
                    await manager.handle_player_ready(websocket)
                else:
                    await manager.set_ready(websocket, message.get("ready", False))
            elif action in ["bet", "call_or_pass", "fold", "all_in"]:
                await manager.handle_action(websocket, action, value)
    except WebSocketDisconnect:
        await manager.disconnect(websocket)
    except Exception as e:
        print(f"WebSocket error: {e}")
        await manager.disconnect(websocket)