import unittest
from models.garden import Garden
from models.plant import Plant
from models.fertilizer import Fertilizer
from models.soil import Soil
from models.seed import Seed
from models.watering import Watering
from models.tool import Tool

class TestGarden(unittest.TestCase):
    def setUp(self):
        self.soil = Soil("Чернозём")
        self.garden = Garden(self.soil)
        self.seed = Seed("Морковь", "Овощ")
        self.garden.plant(self.seed, "Грядка 1")

    def test_plant_growth(self):
        plant = self.garden.plants[self.seed.name]
        self.assertEqual(plant.growth_stage, 0)
        plant.grow()
        self.assertEqual(plant.growth_stage, 1)

    def test_watering(self):
        plant = self.garden.plants[self.seed.name]
        self.assertFalse(plant.watered)
        self.garden.water(self.seed.name)
        self.assertTrue(plant.watered)

    def test_fertilizing(self):
        plant = self.garden.plants[self.seed.name]
        self.assertFalse(plant.fertilized)
        self.garden.fertilize(self.seed.name)
        self.assertTrue(plant.fertilized)
        self.assertEqual(plant.growth_stage, 0.5)

    def test_tool(self):
        name = "Мотыга"
        tool = Tool(name)
        self.assertTrue(tool.name == name)

    def test_harvest(self):
        plant = self.garden.plants[self.seed.name]
        plant.growth_stage = 15
        self.garden.harvest(self.seed.name)
        self.assertTrue(plant.harvested)
        self.assertEqual(plant.growth_stage, 0)

    def test_remove_plant(self):
        self.garden.remove_plant(self.seed.name)
        self.assertNotIn(self.seed.name, self.garden.plants)

    def test_move_plant(self):
        plant = self.garden.plants[self.seed.name]
        self.garden.move_plant(self.seed.name, "Грядка 2")
        self.assertEqual(plant.location, "Грядка 2")

class TestFertilizer(unittest.TestCase):
    def setUp(self):
        self.soil = Soil("Суглинок")
        self.garden = Garden(self.soil)
        self.seed = Seed("Картофель", "Овощ")
        self.garden.plant(self.seed, "Грядка 3")
        self.fertilizer = Fertilizer("Компост")

    def test_fertilizer_usage(self):
        plant = self.garden.plants[self.seed.name]
        self.assertFalse(plant.fertilized)
        self.fertilizer.fertilize_plant(self.garden, self.seed.name)
        self.assertTrue(plant.fertilized)
        self.assertEqual(plant.growth_stage, 0.5)

class TestWatering(unittest.TestCase):
    def setUp(self):
        self.soil = Soil("Песчаная")
        self.garden = Garden(self.soil)
        self.seed = Seed("Капуста", "Овощ")
        self.garden.plant(self.seed, "Грядка 4")
        self.watering = Watering()

    def test_watering_usage(self):
        plant = self.garden.plants[self.seed.name]
        self.assertFalse(plant.watered)
        self.watering.water_plant(self.garden, self.seed.name)
        self.assertTrue(plant.watered)

