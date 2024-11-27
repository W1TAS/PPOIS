#ifndef MANAGER_H
#define MANAGER_H

#include "Employee.h"

class Manager : public Employee {
public:
    Manager(int id, const std::string& first, const std::string& last, const std::string& pos, double rate, double bonus, const ContactInfo& contactInfo, const Address& address);
    double calculateSalary() override;

private:
    double bonus;
};

#endif // MANAGER_H
