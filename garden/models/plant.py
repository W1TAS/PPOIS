class Plant:
    def __init__(self, name: str):
        self.name = name
        self.watered = False
        self.fertilized = False

    def water(self):
        self.watered = True

    def fertilize(self):
        self.fertilized = True

    def __str__(self):
        return f"Растение {self.name}: Полито - {self.watered}, Удобрено - {self.fertilized}"