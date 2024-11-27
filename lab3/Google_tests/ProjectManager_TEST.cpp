#include <gtest/gtest.h>
#include "ProjectManager.h"
#include "Project.h"
#include "Task.h"
#include "Employee.h"
#include "ContactInfo.h"
#include "Address.h"

TEST(ProjectManagerTest, ConstructorTest) {
    ContactInfo contact("123-456-7890", "alice.brown@example.com");
    Address address("Minsk", "Gorkogo", 20);
    ProjectManager pm(3, "Alice", "Brown", "Manager", 60, 500, contact, address);

    EXPECT_EQ(pm.getID(), 3);
    EXPECT_EQ(pm.getFirstName(), "Alice");
    EXPECT_EQ(pm.getLastName(), "Brown");
    EXPECT_EQ(pm.getPosition(), "Manager");
    EXPECT_EQ(pm.getHourlyRate(), 60);
    EXPECT_EQ(pm.getContactInfo().getPhone(), "123-456-7890");
    EXPECT_EQ(pm.getContactInfo().getEmail(), "alice.brown@example.com");
    EXPECT_EQ(pm.getAddress().getCity(), "Minsk");
    EXPECT_EQ(pm.getAddress().getStreet(), "Gorkogo");
    EXPECT_EQ(pm.getAddress().getHouseNumber(), 20);
}

TEST(ProjectManagerTest, ManageProjectTest) {
    ContactInfo contact("123-456-7890", "alice.brown@example.com");
    Address address("Minsk", "Gorkogo", 20);
    ProjectManager pm(3, "Alice", "Brown", "Manager", 60, 500, contact, address);
    Project project("New Project");

    Task task1("Task 1", "2024-11-30");
    Task task2("Task 2", "2024-12-15");
    project.addTask(&task1);
    project.addTask(&task2);

    ContactInfo empContact("123-456-7890", "john.doe@example.com");
    Address empAddress("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, empContact, empAddress);
    project.addEmployee(&emp);

    // Перехват стандартного вывода
    testing::internal::CaptureStdout();
    pm.manageProject(project);
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput =
        "Project: New Project\n"
        "Tasks:\n"
        "- Task 1 (Due: 2024-11-30, Status: Not Completed)\n"
        "- Task 2 (Due: 2024-12-15, Status: Not Completed)\n"
        "Employees:\n"
        "- John Doe\n";

    EXPECT_EQ(output, expectedOutput);
}

