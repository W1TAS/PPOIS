from services.storage import Storage
from services.gardener import Gardener
from models.seed import Seed
from models.tool import Tool
from models.fertilizer import Fertilizer
from models.soil import Soil
from models.garden import Garden
from models.watering import Watering


def main():
    storage = Storage("garden_state.json")
    garden = storage.load()

    if garden is None:
        soil_type = input("Введите тип почвы: ")
        soil = Soil(soil_type)
        garden = Garden(soil)

    gardener = Gardener("Иван")
    watering = Watering()

    while True:
        print("\n1. Посадить растение")
        print("2. Полить растение")
        print("3. Добавить удобрение")
        print("4. Ухаживать за растением")
        print("5. Собрать урожай")
        print("6. Удалить растение")
        print("7. Переместить растение")
        print("8. Вывести состояние огорода")
        print("9. Выйти")

        choice = input("Выберите действие: ")

        if choice == "1":
            plant_name = input("Введите название семени: ")
            plant_type = input("Введите тип растения: ")
            seed = Seed(plant_name, plant_type)
            gardener.plant_seed(garden, seed)
        elif choice == "2":
            plant_name = input("Введите название растения: ")
            gardener.water_plant(garden, plant_name, watering)
        elif choice == "3":
            plant_name = input("Введите название растения: ")
            fertilizer_name = input("Введите название удобрения: ")
            fertilizer = Fertilizer(fertilizer_name)
            gardener.fertilize_plant(garden, plant_name, fertilizer)
        elif choice == "4":
            plant_name = input("Введите название растения: ")
            tool_name = input("Введите название инструмента: ")
            tool = Tool(tool_name)
            gardener.maintain_plant(garden, plant_name, tool)
        elif choice == "5":
            plant_name = input("Введите название растения: ")
            gardener.harvest_plant(garden, plant_name)
        elif choice == "6":
            plant_name = input("Введите название растения для удаления: ")
            gardener.remove_plant(garden, plant_name)
        elif choice == "7":
            plant_name = input("Введите название растения: ")
            new_location = input("Введите новое местоположение: ")
            gardener.move_plant(garden, plant_name, new_location)
        elif choice == "8":
            print(garden)
        elif choice == "9":
            print(f"Сохранение... Тип почвы: {type(garden.soil)}, Значение: {garden.soil}")
            storage.save(garden)
            print("Сохранение и выход...")
            break
        else:
            print("Неверный ввод, попробуйте снова.")


if __name__ == "__main__":
    main()
