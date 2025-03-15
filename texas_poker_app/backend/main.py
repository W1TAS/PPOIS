from fastapi import FastAPI, Request
from starlette.staticfiles import StaticFiles
from starlette.templating import Jinja2Templates
from .routes import game, auth

app = FastAPI()

# Подключение маршрутов
app.include_router(game.router, prefix="/game", tags=["game"])
app.include_router(auth.router, prefix="/auth", tags=["auth"])

# Подключение статических файлов и шаблонов
app.mount("/static", StaticFiles(directory="frontend/static"), name="static")
templates = Jinja2Templates(directory="frontend/templates")

@app.get("/")
async def read_root(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})