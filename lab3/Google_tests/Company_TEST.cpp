#include <gtest/gtest.h>
#include "Company.h"
#include "Department.h"
#include "Employee.h"
#include "Manager.h"

TEST(CompanyTest, ConstructorTest) {
    Company company("Tech Corp");

    EXPECT_EQ(company.getName(), "Tech Corp");
}

TEST(CompanyTest, AddAndRemoveDepartmentTest) {
    Company company("Tech Corp");
    Department dept1("IT");
    Department dept2("HR");

    company.addDepartment(dept1);
    company.addDepartment(dept2);
    EXPECT_EQ(company.getDepartments().size(), 2);

    company.removeDepartment("IT");
    EXPECT_EQ(company.getDepartments().size(), 1);
    EXPECT_EQ(company.getDepartments().at(0).getName(), "HR");
}

TEST(CompanyTest, CalculateTotalSalaryTest) {
    Company company("Tech Corp");
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

    EXPECT_DOUBLE_EQ(company.calculateTotalSalary(), emp1.calculateSalary() + emp2.calculateSalary() + mgr1.calculateSalary());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
