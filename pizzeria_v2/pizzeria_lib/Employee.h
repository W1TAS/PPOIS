#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "Person.h"

class Employee : public Person {
protected:
    int employeeID;
    double salary;

public:
    Employee(const std::string& name, int age, int employeeID, double salary);
    int getEmployeeID() const;
    double getSalary() const;
    virtual void work() = 0;
    virtual double calculateBonus() const = 0;
};

#endif // EMPLOYEE_H
