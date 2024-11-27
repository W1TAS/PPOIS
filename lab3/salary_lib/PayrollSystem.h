#ifndef PAYROLLSYSTEM_H
#define PAYROLLSYSTEM_H

#include "Company.h"

class PayrollSystem {
public:
    PayrollSystem(Company& company);
    void initialize();
    void generateReport() const;

private:
    Company& company;
};

#endif // PAYROLLSYSTEM_H
