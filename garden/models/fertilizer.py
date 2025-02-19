class Fertilizer:
    def __init__(self, name):
        self.name = name

    def fertilize_plant(self, garden, plant_name):
        garden.fertilize(plant_name)