#include <gtest/gtest.h>
#include "Department.h"
#include "Employee.h"
#include "Manager.h"

TEST(DepartmentTest, ConstructorTest) {
    Department dept("IT");

    EXPECT_EQ(dept.getName(), "IT");
    EXPECT_TRUE(dept.getEmployees().empty());
}

TEST(DepartmentTest, AddAndRemoveEmployeeTest) {
    Department dept("IT");
    Employee emp1(1, "John", "Doe", "Developer", 50, ContactInfo("123-456-7890", "john.doe@example.com"), Address("Minsk", "Lenina", 10));
    Employee emp2(2, "Jane", "Smith", "Designer", 45, ContactInfo("098-765-4321", "jane.smith@example.com"), Address("Minsk", "Pushkina", 15));

    dept.addEmployee(&emp1);
    dept.addEmployee(&emp2);

    EXPECT_EQ(dept.getEmployees().size(), 2);

    dept.removeEmployee(1);

    EXPECT_EQ(dept.getEmployees().size(), 1);
    EXPECT_EQ(dept.getEmployees().at(0)->getFirstName(), "Jane");
}

TEST(DepartmentTest, CalculateTotalSalaryTest) {
    Department dept("IT");
    Employee emp1(1, "John", "Doe", "Developer", 50, ContactInfo("123-456-7890", "john.doe@example.com"), Address("Minsk", "Lenina", 10));
    Employee emp2(2, "Jane", "Smith", "Designer", 45, ContactInfo("098-765-4321", "jane.smith@example.com"), Address("Minsk", "Pushkina", 15));
    Manager mgr1(3, "Alice", "Brown", "Manager", 60, 500, ContactInfo("111-222-3333", "alice.brown@example.com"), Address("Minsk", "Gorkogo", 20));

    emp1.updateHoursWorked(160);
    emp2.updateHoursWorked(160);
    mgr1.updateHoursWorked(160);

    dept.addEmployee(&emp1);
    dept.addEmployee(&emp2);
    dept.addEmployee(&mgr1);

    double expectedTotalSalary = emp1.calculateSalary() + emp2.calculateSalary() + mgr1.calculateSalary();
    EXPECT_DOUBLE_EQ(dept.calculateTotalSalary(), expectedTotalSalary);
}


