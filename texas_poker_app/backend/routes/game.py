from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Depends, HTTPException
from ..models.game import PokerGame
from ..routes.auth import get_current_user
from ..models.database import get_db  # Импортируем get_db для ручного разрешения
import json

router = APIRouter()

class ConnectionManager:
    def __init__(self):
        self.active_connections = {}  # {websocket: {"player_id": str, "ready": bool}}
        self.game = None

    async def connect(self, websocket: WebSocket, player_id: str):
        await websocket.accept()
        self.active_connections[websocket] = {"player_id": player_id, "ready": False}
        await self.broadcast_state()

    async def disconnect(self, websocket: WebSocket):
        if websocket in self.active_connections:
            player_id = self.active_connections[websocket]["player_id"]
            del self.active_connections[websocket]
            if not self.active_connections:
                self.game = None
            elif self.game:
                player_index = next((i for i, conn in enumerate(self.active_connections.values()) if conn["player_id"] == player_id), -1)
                if player_index >= 0:
                    self.game.fold(player_index)
                    await self.check_and_advance_stage()
        await self.broadcast_state()

    async def broadcast_state(self):
        if not self.active_connections:
            return
        state = {
            "players": [
                {"player_id": data["player_id"], "ready": data["ready"]}
                for data in self.active_connections.values()
            ],
            "game_active": bool(self.game)
        }
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
        player_names = [data["player_id"] for data in self.active_connections.values()]
        self.game = PokerGame(player_names)
        self.game.start_new_round()
        print("Game started, sending initial state")
        for websocket, data in self.active_connections.items():
            player_index = list(self.active_connections.values()).index(data)
            self.active_connections[websocket]["ready"] = False
            game_state = self.game.get_game_state(player_index)
            print(f"Sending to {data['player_id']}: {game_state}")
            await websocket.send_text(json.dumps(game_state))

    async def set_ready(self, websocket: WebSocket, ready: bool):
        if websocket in self.active_connections:
            self.active_connections[websocket]["ready"] = ready
            await self.broadcast_state()

    async def handle_action(self, websocket: WebSocket, action: str, value=None):
        if not self.game:
            await websocket.send_text(json.dumps({"error": "Game not started, please set ready"}))
            return

        player_index = list(self.active_connections.values()).index(self.active_connections[websocket])
        player_id = self.active_connections[websocket]["player_id"]
        game_state = self.game.get_game_state(player_index)

        if game_state["current_player"] != player_id:
            await websocket.send_text(json.dumps({"error": "Not your turn"}))
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

    async def check_and_advance_stage(self):
        if not self.game:
            return
        game_state = self.game.get_game_state(0)
        active_players = [p for p in game_state["players"] if not p["folded"]]

        if len(active_players) <= 1:
            winner = self.game.get_winner()
            await self.broadcast_winner(winner)
            self.game = None
            await self.broadcast_state()
            return

        if self.game.can_advance_stage():
            if self.game.advance_stage():
                new_game_state = self.game.get_game_state(0)
                if new_game_state["stage"] == "showdown":
                    winner = self.game.get_winner()
                    await self.broadcast_winner(winner)
                    self.game = None
                    await self.broadcast_state()
                    print("Showdown reached, winner determined")
                else:
                    print(f"Automatically advancing to next stage: {new_game_state['stage']}")
        await self.broadcast_game_state()

    async def broadcast_game_state(self):
        if not self.game:
            return
        for websocket, data in self.active_connections.items():
            player_index = list(self.active_connections.values()).index(data)
            game_state = self.game.get_game_state(player_index)
            try:
                await websocket.send_text(json.dumps(game_state))
            except Exception as e:
                print(f"Error sending game state to {data['player_id']}: {e}")

    async def broadcast_winner(self, winner_name):
        for websocket in self.active_connections:
            try:
                await websocket.send_text(json.dumps({"winner": winner_name}))
            except Exception as e:
                print(f"Error sending winner to {self.active_connections[websocket]['player_id']}: {e}")

manager = ConnectionManager()

@router.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket, token: str):
    # Ручное разрешение зависимости get_db
    db = next(get_db())
    try:
        username = await get_current_user(token, db)
    except HTTPException:
        await websocket.close(code=1008)
        return
    finally:
        db.close()  # Закрываем сессию вручную

    await manager.connect(websocket, username)
    try:
        while True:
            data = await websocket.receive_text()
            message = json.loads(data)
            action = message.get("action")
            value = message.get("value")
            if action == "ready":
                await manager.set_ready(websocket, message.get("ready", False))
            elif action in ["bet", "call_or_pass", "fold Tomasz", "all_in"]:
                await manager.handle_action(websocket, action, value)
    except WebSocketDisconnect:
        await manager.disconnect(websocket)
    except Exception as e:
        print(f"WebSocket error: {e}")
        await websocket.close(code=1011)  # Internal error