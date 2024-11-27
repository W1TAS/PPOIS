#include "Company.h"
#include <algorithm>

Company::Company(const std::string& name) : name(name) {}

void Company::addDepartment(const Department& department) {
    departments.push_back(department);
}

void Company::removeDepartment(const std::string& name) {
    departments.erase(std::remove_if(departments.begin(), departments.end(),
                   [name](Department& department) { return department.getName() == name; }), departments.end());
}

double Company::calculateTotalSalary() const {
    double total = 0;
    for (const Department& department : departments) {
        total += department.calculateTotalSalary();
    }
    return total;
}

std::string Company::getName() const { return name; }
const std::vector<Department>& Company::getDepartments() const { return departments; }
