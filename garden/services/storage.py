import json
from pathlib import Path
from typing import Any

STATE_FILE = "garden_state.json"

def save_state(state: dict) -> None:
    """Сохраняет текущее состояние системы в JSON-файл."""
    with open(STATE_FILE, "w", encoding="utf-8") as file:
        json.dump(state, file, indent=4, ensure_ascii=False)

def load_state() -> dict:
    """Загружает состояние системы из JSON-файла. Если файла нет, возвращает пустое состояние."""
    if not Path(STATE_FILE).exists():
        return {}
    with open(STATE_FILE, "r", encoding="utf-8") as file:
        return json.load(file)
