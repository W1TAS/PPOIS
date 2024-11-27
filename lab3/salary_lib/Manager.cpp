#include "Manager.h"

Manager::Manager(int id, const std::string& first, const std::string& last, const std::string& pos, double rate, double bonus, const ContactInfo& contactInfo, const Address& address)
    : Employee(id, first, last, pos, rate, contactInfo, address), bonus(bonus) {}

double Manager::calculateSalary() {
    salary = hourlyRate * hoursWorked + bonus;
    return salary;
}
