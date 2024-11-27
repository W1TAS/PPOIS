#include "Employee.h"
#include "Project.h"

Employee::Employee(int id, const std::string& first, const std::string& last, const std::string& pos, double rate, const ContactInfo& contactInfo, const Address& address)
    : ID(id), firstName(first), lastName(last), position(pos), hourlyRate(rate), hoursWorked(0), salary(0), contactInfo(contactInfo), address(address) {}

double Employee::calculateSalary() {
    salary = hourlyRate * hoursWorked;
    return salary;
}

void Employee::updateHoursWorked(int hours) {
    hoursWorked = hours;
}

int Employee::getID() const { return ID; }
std::string Employee::getFirstName() const { return firstName; }
std::string Employee::getLastName() const { return lastName; }
std::string Employee::getPosition() const { return position; }
double Employee::getHourlyRate() const { return hourlyRate; }
int Employee::getHoursWorked() const { return hoursWorked; }
double Employee::getSalary() const { return salary; }

void Employee::addProject(Project* project) {
    projects.push_back(project);
}

const std::vector<Project*>& Employee::getProjects() const {
    return projects;
}

void Employee::setContactInfo(const ContactInfo& contactInfo) {
    this->contactInfo = contactInfo;
}

void Employee::setAddress(const Address& address) {
    this->address = address;
}

ContactInfo Employee::getContactInfo() const {
    return contactInfo;
}

Address Employee::getAddress() const {
    return address;
}
