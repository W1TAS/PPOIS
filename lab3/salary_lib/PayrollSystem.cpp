#include "PayrollSystem.h"
#include <iostream>

PayrollSystem::PayrollSystem(Company& company) : company(company) {}

void PayrollSystem::initialize() {
    // Инициализация системы расчёта зарплаты
    std::cout << "Payroll system initialized for company: " << company.getName() << std::endl;
}

void PayrollSystem::generateReport() const {
    // Генерация отчёта по зарплатам
    std::cout << "Generating payroll report..." << std::endl;
    std::cout << "Total salary: $" << company.calculateTotalSalary() << std::endl;
}
