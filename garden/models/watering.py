class Irrigation:
    def __init__(self, water_amount: float):
        self.water_amount = water_amount  # in liters

    def water(self, plant):
        print(f"Watering {plant.name} with {self.water_amount} liters of water.")
        plant.grow()
