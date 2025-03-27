from fastapi import FastAPI, Request
from starlette.staticfiles import StaticFiles
from starlette.templating import Jinja2Templates
from .routes import game, auth
import uvicorn

app = FastAPI()

# Подключение маршрутов
app.include_router(game.router, prefix="/game", tags=["game"])
app.include_router(auth.router, prefix="/auth", tags=["auth"])

# Подключение статические файлов и шаблонов
app.mount("/static", StaticFiles(directory="frontend/static"), name="static")
templates = Jinja2Templates(directory="frontend/templates")

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