#ifndef COMPANY_H
#define COMPANY_H

#include <string>
#include <vector>
#include "Department.h"

class Company {
public:
    Company(const std::string& name);
    void addDepartment(const Department& department);
    void removeDepartment(const std::string& name);
    double calculateTotalSalary() const;

    std::string getName() const;
    const std::vector<Department>& getDepartments() const;

private:
    std::string name;
    std::vector<Department> departments;
};

#endif // COMPANY_H
