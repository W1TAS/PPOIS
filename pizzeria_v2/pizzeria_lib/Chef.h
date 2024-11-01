#ifndef CHEF_H
#define CHEF_H

#include "Employee.h"
#include "Pizza.h"
#include "Order.h"

class Chef : public Employee {
private:
    std::string specialty;

public:
    Chef(const std::string& name, int age, int employeeID, double salary, const std::string& specialty);
    std::string getSpecialty() const;
    void prepareOrder(Order& order);
    void createRecipe(const std::string& recipeName);
    double calculateBonus() const override;
    void work() override;
};

#endif // CHEF_H
