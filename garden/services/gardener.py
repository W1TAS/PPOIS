class Gardener:
    def __init__(self, name):
        self.name = name

    def plant_seed(self, garden, seed, location):
        garden.plant(seed, location)

    def maintain_plant(self, garden, plant_name):
        garden.maintain(plant_name)

    def harvest_plant(self, garden, plant_name):
        garden.harvest(plant_name)

    def remove_plant(self, garden, plant_name):
        garden.remove_plant(plant_name)

    def move_plant(self, garden, plant_name, new_location):
        garden.move_plant(plant_name, new_location)