class Gardener:
    def __init__(self, name):
        self.name = name

    def plant_seed(self, garden, seed):
        garden.plant(seed)

    def water_plant(self, garden, plant_name, watering):
        garden.water(plant_name, watering)

    def fertilize_plant(self, garden, plant_name, fertilizer):
        garden.fertilize(plant_name, fertilizer)

    def maintain_plant(self, garden, plant_name, tool):
        garden.maintain(plant_name, tool)

    def harvest_plant(self, garden, plant_name):
        garden.harvest(plant_name)

    def remove_plant(self, garden, plant_name):
        garden.remove_plant(plant_name)

    def move_plant(self, garden, plant_name, new_location):
        garden.move_plant(plant_name, new_location)