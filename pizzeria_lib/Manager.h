#ifndef MANAGER_H
#define MANAGER_H

#include "Employee.h"
#include <vector>

class Manager : public Employee {
private:
    std::vector<Employee*> managedEmployees;

public:
    Manager(const std::string& name, int age, int employeeID, double salary);
    std::vector<Employee*> getManagedEmployees() const;
    void addEmployee(Employee* employee);
    void removeEmployee(Employee* employee);
    double calculateBonus() const override;
    void work() override;
};

#endif // MANAGER_H
