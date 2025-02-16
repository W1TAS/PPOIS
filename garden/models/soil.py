class Soil:
    def __init__(self, type_: str):
        self.type = type_

    def __str__(self):
        return f"Тип почвы: {self.type}"