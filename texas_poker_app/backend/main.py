from fastapi import FastAPI, Request
from starlette.staticfiles import StaticFiles
from starlette.templating import Jinja2Templates
from .routes import game, auth
import uvicorn
import os
app = FastAPI()

# Подключение маршрутов
app.include_router(game.router, prefix="/game", tags=["game"])
app.include_router(auth.router, prefix="/auth", tags=["auth"])

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
STATIC_DIR = os.path.join(BASE_DIR, "frontend", "static")
app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")
templates = Jinja2Templates(directory=os.path.join(BASE_DIR, "frontend", "templates"))

@app.get("/")
async def read_root(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})

# Добавляем инициализацию базы данных при запуске
from .models.database import Base, engine

@app.on_event("startup")
async def startup_event():
    Base.metadata.create_all(bind=engine)  # Создаём таблицы при запуске

if __name__ == "__main__":
    import os
    port = int(os.getenv("PORT", 8000))  # Render задаёт PORT
    uvicorn.run(app, host="0.0.0.0", port=port)