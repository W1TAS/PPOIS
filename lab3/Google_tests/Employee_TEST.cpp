#include <gtest/gtest.h>
#include "Employee.h"
#include "ContactInfo.h"
#include "Address.h"
#include "Project.h"

TEST(EmployeeTest, ConstructorTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);

    EXPECT_EQ(emp.getID(), 1);
    EXPECT_EQ(emp.getFirstName(), "John");
    EXPECT_EQ(emp.getLastName(), "Doe");
    EXPECT_EQ(emp.getPosition(), "Developer");
    EXPECT_EQ(emp.getHourlyRate(), 50);
    EXPECT_EQ(emp.getContactInfo().getPhone(), "123-456-7890");
    EXPECT_EQ(emp.getContactInfo().getEmail(), "john.doe@example.com");
    EXPECT_EQ(emp.getAddress().getCity(), "Minsk");
    EXPECT_EQ(emp.getAddress().getStreet(), "Lenina");
    EXPECT_EQ(emp.getAddress().getHouseNumber(), 10);
}

TEST(EmployeeTest, UpdateHoursWorkedTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);

    emp.updateHoursWorked(160);
    EXPECT_EQ(emp.getHoursWorked(), 160);
}

TEST(EmployeeTest, CalculateSalaryTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);

    emp.updateHoursWorked(160);
    EXPECT_DOUBLE_EQ(emp.calculateSalary(), 8000); // 50 * 160
    EXPECT_DOUBLE_EQ(emp.getSalary(), 8000);
}

TEST(EmployeeTest, AddProjectTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);
    Project project("New Project");

    emp.addProject(&project);
    EXPECT_EQ(emp.getProjects().size(), 1);
    EXPECT_EQ(emp.getProjects()[0]->getName(), "New Project");
}

TEST(EmployeeTest, SetContactInfoTest) {
    ContactInfo oldContact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, oldContact, address);

    ContactInfo newContact("098-765-4321", "john.new@example.com");
    emp.setContactInfo(newContact);
    EXPECT_EQ(emp.getContactInfo().getPhone(), "098-765-4321");
    EXPECT_EQ(emp.getContactInfo().getEmail(), "john.new@example.com");
}

TEST(EmployeeTest, SetAddressTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address oldAddress("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, oldAddress);

    Address newAddress("Minsk", "Pushkina", 20);
    emp.setAddress(newAddress);
    EXPECT_EQ(emp.getAddress().getCity(), "Minsk");
    EXPECT_EQ(emp.getAddress().getStreet(), "Pushkina");
    EXPECT_EQ(emp.getAddress().getHouseNumber(), 20);
}


