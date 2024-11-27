#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <vector>
#include "ContactInfo.h"
#include "Address.h"

class Project; // Forward declaration

class Employee {
public:
    Employee(int id, const std::string& first, const std::string& last, const std::string& pos, double rate, const ContactInfo& contactInfo, const Address& address);
    virtual ~Employee() = default;

    virtual double calculateSalary();
    void updateHoursWorked(int hours);

    int getID() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getPosition() const;
    double getHourlyRate() const;
    int getHoursWorked() const;
    double getSalary() const;

    void addProject(Project* project);
    const std::vector<Project*>& getProjects() const;

    // ContactInfo and Address
    void setContactInfo(const ContactInfo& contactInfo);
    void setAddress(const Address& address);
    ContactInfo getContactInfo() const;
    Address getAddress() const;

protected:
    int ID;
    std::string firstName;
    std::string lastName;
    std::string position;
    double hourlyRate;
    int hoursWorked;
    double salary;
    std::vector<Project*> projects; // Ассоциация с проектами

    ContactInfo contactInfo; // Ассоциация с контактной информацией
    Address address; // Ассоциация с адресом
};

#endif // EMPLOYEE_H
