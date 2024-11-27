#include "Department.h"
#include <algorithm>

Department::Department(const std::string& name) : name(name) {}

void Department::addEmployee(Employee* employee) {
    employees.push_back(employee);
}

void Department::removeEmployee(int id) {
    employees.erase(std::remove_if(employees.begin(), employees.end(),
                   [id](Employee* employee) { return employee->getID() == id; }), employees.end());
}

double Department::calculateTotalSalary() const {
    double total = 0;
    for (Employee* employee : employees) {
        total += employee->calculateSalary();
    }
    return total;
}

std::string Department::getName() const { return name; }
const std::vector<Employee*>& Department::getEmployees() const { return employees; }
