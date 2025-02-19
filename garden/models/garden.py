import json
from models.soil import Soil
from models.plant import Plant

class Garden:
    def __init__(self, soil: Soil):
        self.plants = {}  # Теперь словарь хранит объекты Plant
        self.soil = soil

    def plant(self, seed, location):
        """ Создает новое растение в огороде. """
        new_plant = Plant(seed.name, seed.plant_type, location)
        self.plants[seed.name] = new_plant

    def grow(self):
        """ Проходит неделя, растения растут, но могут засохнуть. """
        to_remove = [name for name, plant in self.plants.items() if not plant.grow()]
        for name in to_remove:
            del self.plants[name]  # Удаляем засохшие растения

    def water(self, name):
        if name in self.plants:
            self.plants[name].water()

    def fertilize(self, name):
        if name in self.plants:
            self.plants[name].fertilize()

    def maintain(self, name):
        if name in self.plants:
            self.plants[name].maintain()

    def harvest(self, name):
        if name in self.plants:
            self.plants[name].harvest()

    def remove_plant(self, name):
        if name in self.plants:
            del self.plants[name]

    def move_plant(self, name, new_location):
        if name in self.plants:
            self.plants[name].move(new_location)

    def __str__(self):
        return json.dumps(
            {name: plant.__dict__ for name, plant in self.plants.items()},
            indent=4, ensure_ascii=False
        )
