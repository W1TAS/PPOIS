#include "Manager.h"
#include <algorithm>
#include <iostream>

Manager::Manager(const std::string& name, int age, int employeeID, double salary)
    : Employee(name, age, employeeID, salary) {}

std::vector<Employee*> Manager::getManagedEmployees() const {
    return managedEmployees;
}

void Manager::addEmployee(Employee* employee) {
    managedEmployees.push_back(employee);
}

void Manager::removeEmployee(Employee* employee) {
    auto it = std::find(managedEmployees.begin(), managedEmployees.end(), employee);
    if (it != managedEmployees.end()) {
        managedEmployees.erase(it);
        std::cout << "Работник устранён." << std::endl;
    } else {
        std::cout << "Работник не найден." << std::endl;
    }
}

double Manager::calculateBonus() const {
    return salary * 0.1; // Пример расчета бонуса
}

void Manager::work() {
    std::cout << "Менеджер работает." << std::endl;
}
