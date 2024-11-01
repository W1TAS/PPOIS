#include "Chef.h"
#include <iostream>

Chef::Chef(const std::string& name, int age, int employeeID, double salary, const std::string& specialty)
    : Employee(name, age, employeeID, salary), specialty(specialty) {}

std::string Chef::getSpecialty() const {
    return specialty;
}

void Chef::prepareOrder(Order& order) {

    order.setStatus("In Progress");

    for (auto& pizza : order.getPizzas()) {
        std::cout << "Шеф-повар " << name << " готовит пиццу с размером " << pizza.getSize()
                  << " и типом корочки " << pizza.getCrustType() << " с топпингами:";
        for (const auto& topping : pizza.getToppings()) {
            std::cout << " " << topping;
        }
        std::cout << "." << std::endl;
    }
    order.setStatus("Completed");
    std::cout << "Все пиццы в заказе приготовлены. Статус заказа: " << order.getStatus() << "." << std::endl;
}

void Chef::createRecipe(const std::string& recipeName) {
    std::cout << "Шеф-повар " << name << " создал новый рецепт: " << recipeName << "." << std::endl;
}

double Chef::calculateBonus() const {
    return salary * 0.15;
}

void Chef::work() {
    createRecipe("Специальное блюдо от шефа");
    std::cout << "Шеф-повар " << name << " готовит блюда и управляет кухней." << std::endl;
}
