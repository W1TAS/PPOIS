class Seed:
    def __init__(self, plant_type: str):
        self.plant_type = plant_type

    def __str__(self):
        return f"Семена {self.plant_type}"