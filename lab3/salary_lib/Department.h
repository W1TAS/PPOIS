#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <string>
#include <vector>
#include "Employee.h"

class Department {
public:
    Department(const std::string& name);
    void addEmployee(Employee* employee);
    void removeEmployee(int id);
    double calculateTotalSalary() const;

    std::string getName() const;
    const std::vector<Employee*>& getEmployees() const;

private:
    std::string name;
    std::vector<Employee*> employees;
};

#endif // DEPARTMENT_H
