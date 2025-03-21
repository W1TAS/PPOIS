from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Depends, HTTPException
from ..models.game import PokerGame, Player
from ..routes.auth import get_current_user
from ..models.database import get_db
import json
import asyncio

router = APIRouter()

class ConnectionManager:
    def __init__(self):
        self.active_connections = {}  # {websocket: {"player_id": str, "ready": bool, "index": int}}
        self.game = None
        self.players = {}  # {player_id: Player}
        self.last_dealer_index = -1  # Добавляем поле для хранения последнего dealer_index

    async def connect(self, websocket: WebSocket, player_id: str):
        await websocket.accept()
        if player_id not in self.players:
            self.players[player_id] = Player(player_id)
        self.active_connections[websocket] = {"player_id": player_id, "ready": False, "index": None}
        await self.broadcast_state()

    async def disconnect(self, websocket: WebSocket):
        if websocket in self.active_connections:
            player_id = self.active_connections[websocket]["player_id"]
            player_index = self.active_connections[websocket]["index"]
            del self.active_connections[websocket]
            print(f"Player {player_id} (index {player_index}) disconnected")

            # Если нет активных подключений, сбрасываем игру
            if not self.active_connections:
                self.game = None
                self.last_dealer_index = -1
                print("No active connections, game reset")
                return

            # Если игра запущена и игрок участвовал
            if self.game and player_index >= 0:
                # Игрок сбрасывает карты
                self.game.players[player_index].fold()
                print(f"Player {player_id} folded due to disconnect")

                # Проверяем, остались ли активные игроки
                active_players = [p for p in self.game.players if not p.folded and p.balance > 0]
                if len(active_players) <= 1:
                    print("Only one active player left, ending game")
                    winner = self.game.get_winner()
                    await self.broadcast_winner(winner, True)
                    return

                # Обновляем текущего игрока
                self.game.current_player_index = self.game.next_active_player(self.game.current_player_index)
                await self.check_and_advance_stage()

            # Обновляем индексы в active_connections
            active_player_ids = [p.name for p in self.game.players] if self.game else []
            for ws, data in self.active_connections.items():
                if data["player_id"] in active_player_ids:
                    data["index"] = active_player_ids.index(data["player_id"])
                else:
                    data["index"] = -1  # Игрок не участвует в игре

            await self.broadcast_state()

    async def broadcast_state(self):
        if not self.active_connections:
            return
        num_players = len(self.active_connections)
        state = {
            "players": [
                {
                    "player_id": data["player_id"],
                    "ready": data["ready"],
                    "balance": self.players[data["player_id"]].balance,
                    "is_dealer": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 1) % num_players,  # Следующий дилер
                    "is_small_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 2) % num_players,
                    "is_big_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 3) % num_players,
                }
                for idx, data in enumerate(self.active_connections.values())
            ],
            "game_active": bool(self.game)
        }
        print(f"Broadcasting state: {state}")  # Добавляем лог для отладки
        all_ready = all(data["ready"] for data in self.active_connections.values())
        if all_ready and len(self.active_connections) > 1 and not self.game:
            await self.start_game()
        else:
            for websocket in self.active_connections:
                try:
                    await websocket.send_text(json.dumps(state))
                except Exception as e:
                    print(f"Error sending state to {self.active_connections[websocket]['player_id']}: {e}")

    async def start_game(self):
        # Получаем ID игроков и фильтруем тех, у кого баланс > 0
        player_ids = [data["player_id"] for data in self.active_connections.values()]
        active_player_ids = [pid for pid in player_ids if self.players[pid].balance > 0]
        print(f"Player IDs in start_game: {player_ids}")
        print(f"Active player IDs after filtering: {active_player_ids}")

        # Проверяем, достаточно ли игроков для начала игры
        if len(active_player_ids) < 2:
            print("Not enough players with balance to start the game")
            for websocket in self.active_connections:
                await websocket.send_text(json.dumps({"error": "Not enough players with balance to start the game"}))
            self.game = None
            return

        # Сохраняем текущий dealer_index, если игра уже была
        if self.game:
            self.last_dealer_index = self.game.dealer_index
        print(f"Last dealer_index before new game: {self.last_dealer_index}")

        # Создаём новую игру только с активными игроками
        self.game = PokerGame([])
        self.game.players = [self.players[player_id] for player_id in active_player_ids]

        # Устанавливаем dealer_index перед началом нового раунда
        if self.last_dealer_index >= 0:
            self.game.dealer_index = self.last_dealer_index
        else:
            self.game.dealer_index = 0  # Устанавливаем начальное значение, если игра новая

        # Запускаем новый раунд с обработкой исключений
        try:
            self.game.start_new_round()
        except ValueError as e:
            print(f"Error starting new round: {e}")
            for websocket in self.active_connections:
                await websocket.send_text(json.dumps({"error": str(e)}))
            self.game = None
            return

        print("Game started, sending initial state")
        for idx, (websocket, data) in enumerate(self.active_connections.items()):
            # Устанавливаем индекс только для активных игроков
            if data["player_id"] in active_player_ids:
                data["index"] = active_player_ids.index(data["player_id"])
            else:
                data["index"] = -1  # Игроки с нулевым балансом не участвуют
            self.active_connections[websocket]["ready"] = False
            if data["index"] >= 0:
                game_state = self.game.get_game_state(data["index"])
                print(f"Sending to {data['player_id']} (index {data['index']}): {game_state}")
                await websocket.send_text(json.dumps(game_state))
            else:
                await websocket.send_text(json.dumps({"error": "You cannot participate because your balance is 0"}))

    async def set_ready(self, websocket: WebSocket, ready: bool):
        if websocket in self.active_connections:
            self.active_connections[websocket]["ready"] = ready
            await self.broadcast_state()

    async def handle_action(self, websocket: WebSocket, action: str, value=None):
        if not self.game:
            await websocket.send_text(json.dumps({"error": "Game not started, please set ready"}))
            return

        player_data = self.active_connections[websocket]
        player_index = player_data["index"]
        player_id = player_data["player_id"]
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

            # После каждого действия вызываем check_and_advance_stage, пока стадия меняется
            previous_stage = self.game.stage
            while True:
                await self.check_and_advance_stage()
                current_stage = self.game.stage
                if current_stage == previous_stage:
                    break  # Если стадия не изменилась, выходим из цикла
                previous_stage = current_stage

        except ValueError as e:
            await websocket.send_text(json.dumps({"error": str(e)}))
            await self.broadcast_game_state()


    async def handle_player_ready(self, websocket: WebSocket):
        if not self.game:
            return
        player_data = self.active_connections[websocket]
        player_id = player_data["player_id"]
        self.active_connections[websocket]["ready"] = True
        all_ready = all(data["ready"] for data in self.active_connections.values())
        if all_ready:
            # Сохраняем dealer_index перед сбросом игры
            self.last_dealer_index = self.game.dealer_index
            self.game = None  # Сбрасываем игру
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
            print(f"All ready state: {state}")  # Добавляем лог для отладки
            for ws in self.active_connections:
                try:
                    await ws.send_text(json.dumps(state))
                except Exception as e:
                    print(f"Error sending all_ready state to {self.active_connections[ws]['player_id']}: {e}")
            for ws in self.active_connections:
                self.active_connections[ws]["ready"] = False
            # Запускаем новую игру
            await self.start_game()
        else:
            for ws, data in self.active_connections.items():
                player_index = data["index"]  # Используем сохранённый индекс
                if player_index < 0:  # Игрок не участвует в игре
                    continue
                game_state = self.game.get_game_state(player_index)
                # Добавляем ready в game_state
                for i, player in enumerate(game_state["players"]):
                    player["balance"] = self.players[player["name"]].balance
                    player["ready"] = list(self.active_connections.values())[i]["ready"]
                try:
                    await ws.send_text(json.dumps(game_state))
                except Exception as e:
                    print(f"Error sending ready state to {self.active_connections[ws]['player_id']}: {e}")

    async def check_and_advance_stage(self):
        if not self.game:
            return
        game_state = self.game.get_game_state(0)
        active_players = [p for p in game_state["players"] if not p["folded"] and self.players[p["name"]].balance > 0]
        players_in_game = [p for p in game_state["players"] if not p["folded"]]
        all_folded = len(players_in_game) <= 1 and any(p["folded"] for p in game_state["players"])

        print(
            f"Checking stage: {self.game.stage}, active_players: {len(active_players)}, players_in_game: {len(players_in_game)}")

        if self.game.stage == "showdown":
            winner = self.game.get_winner()
            print(f"Winner determined: {winner}")
            await self.broadcast_winner(winner, all_folded)
            return

        if len(players_in_game) <= 1:
            while self.game.stage != "showdown" and self.game.advance_stage():
                new_game_state = self.game.get_game_state(0)
                print(f"Automatically advancing to next stage: {new_game_state['stage']}")
            if self.game.stage == "showdown":
                winner = self.game.get_winner()
                print(f"Winner determined: {winner}")
                await self.broadcast_winner(winner, all_folded)
                return
            await self.broadcast_game_state()
            return

        if len(active_players) <= 1:
            max_bet = max(p["bet"] for p in players_in_game)
            all_bets_settled = all(p["bet"] == max_bet or self.players[p["name"]].balance == 0 for p in players_in_game)
            if all_bets_settled:
                while self.game.stage != "showdown" and self.game.advance_stage():
                    new_game_state = self.game.get_game_state(0)
                    print(f"Automatically advancing to next stage: {new_game_state['stage']}")
                if self.game.stage == "showdown":
                    winner = self.game.get_winner()
                    print(f"Winner determined: {winner}")
                    await self.broadcast_winner(winner, all_folded)
                    return
                await self.broadcast_game_state()
                return

        if self.game.can_advance_stage():
            if self.game.advance_stage():
                new_game_state = self.game.get_game_state(0)
                print(f"Stage advanced to: {new_game_state['stage']}")
                if new_game_state["stage"] == "showdown":
                    winner = self.game.get_winner()
                    print(f"Winner determined: {winner}")
                    await self.broadcast_winner(winner, all_folded)
                    return
                await self.broadcast_game_state()
            else:
                print("Cannot advance stage after advance_stage call, broadcasting current state")
                await self.broadcast_game_state()
        else:
            if self.game.stage == "showdown":
                winner = self.game.get_winner()
                print(f"Winner determined: {winner}")
                await self.broadcast_winner(winner, all_folded)
                return
            print("Cannot advance stage, broadcasting current state")
            await self.broadcast_game_state()

    async def broadcast_game_state(self):
        if not self.game:
            return
        for websocket, data in self.active_connections.items():
            player_index = data["index"]  # Используем сохранённый индекс
            if player_index < 0:  # Игрок не участвует в игре (баланс 0)
                continue
            game_state = self.game.get_game_state(player_index)
            # Убедимся, что balance присутствует для всех игроков
            for player in game_state["players"]:
                player["balance"] = self.players[player["name"]].balance
                # Добавляем ready только в состоянии showdown
                if self.game.stage == "showdown":
                    player["ready"] = self.active_connections[websocket]["ready"]
            print(f"Sending game state to {data['player_id']} (index {player_index}): {game_state}")
            try:
                await websocket.send_text(json.dumps(game_state))
            except Exception as e:
                print(f"Error sending game state to {data['player_id']}: {e}")

    async def broadcast_winner(self, winner_data, all_folded):
        state = {
            "winner": winner_data,
            "all_folded": all_folded,
        }
        for websocket, data in self.active_connections.items():
            player_index = data["index"]  # Используем сохранённый индекс
            if player_index < 0:  # Игрок не участвует в игре
                continue
            game_state = self.game.get_game_state(player_index)
            # Добавляем карты всех игроков в game_state для showdown
            game_state["players"] = [
                {
                    "name": p.name,
                    "folded": p.folded,
                    "bet": p.current_bet,
                    "balance": self.players[p.name].balance,
                    "hand": [str(card) for card in p.hand] if self.game.stage == "showdown" else None,
                    "is_dealer": i == self.game.dealer_index,
                    "is_small_blind": i == (self.game.dealer_index + 1) % len(self.game.players),
                    "is_big_blind": i == (self.game.dealer_index + 2) % len(self.game.players),
                    "ready": self.active_connections[websocket]["ready"],
                }
                for i, p in enumerate(self.game.players)
            ]
            game_state.update(state)  # Обновляем состояние с информацией о победителе
            try:
                await websocket.send_text(json.dumps(game_state))
            except Exception as e:
                print(f"Error sending winner to {self.active_connections[websocket]['player_id']}: {e}")

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
        await websocket.close(code=1011)