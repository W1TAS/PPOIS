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
            del self.active_connections[websocket]
            if not self.active_connections:
                self.game = None
                self.last_dealer_index = -1  # Сбрасываем last_dealer_index
            elif self.game:
                player_index = next((i for i, conn in enumerate(self.active_connections.values()) if conn["player_id"] == player_id), -1)
                if player_index >= 0:
                    self.game.fold(player_index)
                    await self.check_and_advance_stage()
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
        player_ids = [data["player_id"] for data in self.active_connections.values()]
        print(f"Player IDs in start_game: {player_ids}")

        # Сохраняем текущий dealer_index, если игра уже была
        if self.game:
            self.last_dealer_index = self.game.dealer_index
        print(f"Last dealer_index before new game: {self.last_dealer_index}")

        # Создаём новую игру
        self.game = PokerGame([])
        self.game.players = [self.players[player_id] for player_id in player_ids]

        # Устанавливаем dealer_index перед началом нового раунда
        if self.last_dealer_index >= 0:
            self.game.dealer_index = self.last_dealer_index
        else:
            self.game.dealer_index = 0  # Устанавливаем начальное значение, если игра новая

        # Запускаем новый раунд
        self.game.start_new_round()

        print("Game started, sending initial state")
        for idx, (websocket, data) in enumerate(self.active_connections.items()):
            data["index"] = idx
            self.active_connections[websocket]["ready"] = False
            game_state = self.game.get_game_state(idx)
            print(f"Sending to {data['player_id']} (index {idx}): {game_state}")
            await websocket.send_text(json.dumps(game_state))

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
                if self.game.place_bet(player_index, value):
                    await self.check_and_advance_stage()
            elif action == "call_or_pass":
                if self.game.call_or_pass(player_index):
                    await self.check_and_advance_stage()
            elif action == "fold":
                if self.game.fold(player_index):
                    await self.check_and_advance_stage()
            elif action == "all_in":
                if self.game.all_in(player_index):
                    await self.check_and_advance_stage()
        except ValueError as e:
            await websocket.send_text(json.dumps({"error": str(e)}))
            await self.broadcast_game_state()

    # routes/game.py (фрагмент)

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
                        "is_dealer": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 1) % len(self.active_connections),
                        "is_small_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 2) % len(self.active_connections),
                        "is_big_blind": self.last_dealer_index >= 0 and idx == (self.last_dealer_index + 3) % len(self.active_connections),
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
                player_index = list(self.active_connections.values()).index(data)
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
        active_players = [p for p in game_state["players"] if not p["folded"]]

        all_folded = len(active_players) <= 1 and any(p["folded"] for p in game_state["players"])

        if len(active_players) <= 1:
            winner = self.game.get_winner()
            await self.broadcast_winner(winner, all_folded)
            return

        if self.game.can_advance_stage():
            if self.game.advance_stage():
                new_game_state = self.game.get_game_state(0)
                if new_game_state["stage"] == "showdown":
                    winner = self.game.get_winner()
                    await self.broadcast_winner(winner, all_folded)
                    return
                else:
                    print(f"Automatically advancing to next stage: {new_game_state['stage']}")
            await self.broadcast_game_state()
        else:
            print("Cannot advance stage, broadcasting current state")
            await self.broadcast_game_state()

    # routes/game.py (фрагмент)

    async def broadcast_game_state(self):
        if not self.game:
            return
        for websocket, data in self.active_connections.items():
            player_index = list(self.active_connections.values()).index(data)
            game_state = self.game.get_game_state(player_index)
            # Убедимся, что balance присутствует для всех игроков
            for player in game_state["players"]:
                player["balance"] = self.players[player["name"]].balance
                # Добавляем ready только в состоянии showdown
                if self.game.stage == "showdown":
                    player["ready"] = self.active_connections[list(self.active_connections.keys())[player_index]][
                        "ready"]
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
            player_index = list(self.active_connections.values()).index(data)
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
                    "ready": self.active_connections[list(self.active_connections.keys())[i]]["ready"],
                    # Добавляем статус ready
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