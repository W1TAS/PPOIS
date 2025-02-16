import json
from pathlib import Path
from models.garden import Garden
from models.soil import Soil


class Storage:
    def __init__(self, filename):
        self.filepath = Path(filename)

    def load(self):
        if self.filepath.exists():
            with open(self.filepath, "r", encoding="utf-8") as file:
                data = json.load(file)
                if "soil" not in data:  # Если нет информации о почве
                    return None
                soil = Soil(data["soil"])  # Создаём объект почвы
                garden = Garden(soil)
                garden.plants = data.get("plants", {})
                return garden
        return None

    def save(self, garden):
        with open(self.filepath, "w", encoding="utf-8") as file:
            json.dump({
                "soil": garden.soil.soil_type,  # Сохраняем строку, а не объект
                "plants": garden.plants
            }, file, indent=4, ensure_ascii=False)

