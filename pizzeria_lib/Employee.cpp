#include "Employee.h"

Employee::Employee(const std::string& name, int age, int employeeID, double salary)
    : Person(name, age), employeeID(employeeID), salary(salary) {}

int Employee::getEmployeeID() const {
    return employeeID;
}

double Employee::getSalary() const {
    return salary;
}
