import json
from models.soil import Soil

class Garden:
    def __init__(self, soil: Soil):
        self.plants = {}
        self.soil = soil  # Храним объект, а не строку

    def __str__(self):
        return json.dumps({
            "soil": self.soil.soil_type,  # Преобразуем в строку перед выводом
            "plants": self.plants
        }, indent=4, ensure_ascii=False)

    def plant(self, seed):
        self.plants[seed.name] = {
            "type": seed.plant_type,
            "watered": False,
            "fertilized": False,
            "harvested": False,
            "maintained": False,
            "location": "default"
        }

    def water(self, name, watering):
        if name in self.plants:
            self.plants[name]["watered"] = watering.water()

    def fertilize(self, name, fertilizer):
        if name in self.plants:
            self.plants[name]["fertilized"] = True

    def maintain(self, name, tool):
        if name in self.plants:
            self.plants[name]["maintained"] = True

    def harvest(self, name):
        if name in self.plants:
            self.plants[name]["harvested"] = True

    def remove_plant(self, name):
        if name in self.plants:
            del self.plants[name]

    def move_plant(self, name, new_location):
        if name in self.plants:
            self.plants[name]["location"] = new_location

    def __str__(self):
        return json.dumps(self.plants, indent=4, ensure_ascii=False)