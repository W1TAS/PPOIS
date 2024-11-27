#include <gtest/gtest.h>
#include "PayrollSystem.h"
#include "Company.h"
#include "Department.h"
#include "Employee.h"
#include "Manager.h"

TEST(PayrollSystemTest, InitializeTest) {
    Company company("Tech Corp");
    PayrollSystem payrollSystem(company);

    // Перехват стандартного вывода
    testing::internal::CaptureStdout();
    payrollSystem.initialize();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "Payroll system initialized for company: Tech Corp\n");
}

TEST(PayrollSystemTest, GenerateReportTest) {
    Company company("Tech Corp");
    PayrollSystem payrollSystem(company);

    Department dept1("IT");
    Employee emp1(1, "John", "Doe", "Developer", 50, ContactInfo("123-456-7890", "john.doe@example.com"), Address("Minsk", "Lenina", 10));
    Employee emp2(2, "Jane", "Smith", "Designer", 45, ContactInfo("098-765-4321", "jane.smith@example.com"), Address("Minsk", "Pushkina", 15));
    Manager mgr1(3, "Alice", "Brown", "Manager", 60, 500, ContactInfo("111-222-3333", "alice.brown@example.com"), Address("Minsk", "Gorkogo", 20));

    emp1.updateHoursWorked(160);
    emp2.updateHoursWorked(160);
    mgr1.updateHoursWorked(160);

    dept1.addEmployee(&emp1);
    dept1.addEmployee(&emp2);
    dept1.addEmployee(&mgr1);
    company.addDepartment(dept1);

    // Перехват стандартного вывода
    testing::internal::CaptureStdout();
    payrollSystem.generateReport();
    std::string output = testing::internal::GetCapturedStdout();

    double totalSalary = emp1.calculateSalary() + emp2.calculateSalary() + mgr1.calculateSalary();
    std::ostringstream expectedOutput;
    expectedOutput << "Generating payroll report...\nTotal salary: $" << totalSalary << "\n";

    EXPECT_EQ(output, expectedOutput.str());
}

