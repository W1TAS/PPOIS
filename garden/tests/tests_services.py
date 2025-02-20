import unittest
import json
import os
from models.soil import Soil
from models.garden import Garden
from models.plant import Plant
from models.seed import Seed
from services.gardener import Gardener
from services.storage import Storage


class TestGardener(unittest.TestCase):
    def setUp(self):
        self.soil = Soil("Чернозём")
        self.garden = Garden(self.soil)
        self.gardener = Gardener("Иван")
        self.seed = Seed("Морковь", "Овощ")
        self.gardener.plant_seed(self.garden, self.seed, "Грядка 1")

    def test_plant_seed(self):
        self.assertIn("Морковь", self.garden.plants)
        self.assertEqual(self.garden.plants["Морковь"].location, "Грядка 1")

    def test_maintain_plant(self):
        self.gardener.maintain_plant(self.garden, "Морковь")
        self.assertTrue(self.garden.plants["Морковь"].maintained)

    def test_harvest_plant(self):
        self.gardener.harvest_plant(self.garden, "Морковь")
        self.assertTrue(self.garden.plants["Морковь"].harvested)
        self.assertEqual(self.garden.plants["Морковь"].growth_stage, 0)

    def test_remove_plant(self):
        self.gardener.remove_plant(self.garden, "Морковь")
        self.assertNotIn("Морковь", self.garden.plants)

    def test_move_plant(self):
        self.gardener.move_plant(self.garden, "Морковь", "Грядка 2")
        self.assertEqual(self.garden.plants["Морковь"].location, "Грядка 2")


class TestStorage(unittest.TestCase):
    def setUp(self):
        self.soil = Soil("Суглинок")
        self.garden = Garden(self.soil)
        self.storage = Storage("test_garden.json")
        self.seed = Seed("Помидор", "Овощ")
        self.garden.plant(self.seed, "Грядка 3")

    def tearDown(self):
        if os.path.exists("test_garden.json"):
            os.remove("test_garden.json")

    def test_save_and_load(self):
        self.storage.save(self.garden)
        loaded_garden = self.storage.load()

        self.assertIsNotNone(loaded_garden)
        self.assertEqual(loaded_garden.soil.soil_type, "Суглинок")
        self.assertIn("Помидор", loaded_garden.plants)
        self.assertEqual(loaded_garden.plants["Помидор"].location, "Грядка 3")


if __name__ == "__main__":
    unittest.main()
