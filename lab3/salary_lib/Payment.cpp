#include "Payment.h"
#include <iostream>

Payment::Payment(Employee* employee, double amount, const std::string& date)
    : employee(employee), amount(amount), date(date) {
    if (amount < 0) {
        throw std::invalid_argument("Payment amount cannot be negative");
    }
}

void Payment::processPayment() {
    std::cout << "Processed payment of $" << amount << " to " << employee->getFirstName() << " " << employee->getLastName() << " on " << date << std::endl;
}

Employee* Payment::getEmployee() const { return employee; }
double Payment::getAmount() const { return amount; }
std::string Payment::getDate() const { return date; }
