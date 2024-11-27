#ifndef PAYMENT_H
#define PAYMENT_H

#include "Employee.h"
#include <string>

class Payment {
public:
    Payment(Employee* employee, double amount, const std::string& date);
    void processPayment();

    Employee* getEmployee() const;
    double getAmount() const;
    std::string getDate() const;

private:
    Employee* employee;
    double amount;
    std::string date;
};

#endif // PAYMENT_H
