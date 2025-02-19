import json
from models.soil import Soil
from models.garden import Garden
from models.plant import Plant

class Storage:
    def __init__(self, filename):
        self.filename = filename

    def save(self, garden):
        """Сохраняет состояние огорода в JSON."""
        with open(self.filename, "w", encoding="utf-8") as f:
            json.dump({
                "soil": garden.soil.soil_type,  # Сохраняем тип почвы
                "plants": {name: plant.__dict__ for name, plant in garden.plants.items()}  # Преобразуем объекты Plant в словари
            }, f, indent=4, ensure_ascii=False)

    def load(self):
        """Загружает состояние огорода из JSON."""
        try:
            with open(self.filename, "r", encoding="utf-8") as f:
                data = json.load(f)

            soil = Soil(data["soil"])
            garden = Garden(soil)

            for name, plant_data in data["plants"].items():
                plant = Plant(**plant_data)  # Создаём объект из словаря
                garden.plants[name] = plant

            return garden
        except (FileNotFoundError, json.JSONDecodeError):
            return None
