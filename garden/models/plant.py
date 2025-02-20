class Plant:
    MAX_GROWTH_STAGE = 15
    MAX_DAYS_WITHOUT_WATER = 7

    def __init__(self, name: str, plant_type: str, location: str):
        self.name = name
        self.plant_type = plant_type
        self.location = location
        self.growth_stage = 0
        self.watered = False
        self.fertilized = False
        self.harvested = False
        self.maintained = False
        self.days_without_water = 0

    def grow(self):
        """ Увеличивает степень роста и засухи, удаляет растение при засыхании. """
        if self.growth_stage < self.MAX_GROWTH_STAGE:
            self.growth_stage += 1
        self.days_without_water += 1

        if self.days_without_water >= self.MAX_DAYS_WITHOUT_WATER:
            print(f"Растение {self.name} засохло и погибло!")
            return False  # Сообщаем, что растение засохло и должно быть удалено
        return True  # Растение живо

    def water(self):
        """ Полив сбрасывает таймер засухи. """
        self.watered = True
        self.days_without_water = 0

    def fertilize(self):
        """ Удобрение ускоряет рост. """
        self.fertilized = True
        if self.growth_stage < self.MAX_GROWTH_STAGE:
            self.growth_stage += 0.5

    def maintain(self):
        """ Обработка инструмента. """
        self.maintained = True

    def harvest(self):
        """ Сбор урожая обнуляет рост. """
        if self.growth_stage >= self.MAX_GROWTH_STAGE:
            self.harvested = True
            self.growth_stage = 0

    def move(self, new_location: str):
        """ Перемещение растения. """
        self.location = new_location

    def __repr__(self):
        return f"{self.name} ({self.plant_type}) - стадия {self.growth_stage}, местоположение: {self.location}"
