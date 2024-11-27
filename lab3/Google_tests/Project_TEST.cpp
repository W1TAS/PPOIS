#include <gtest/gtest.h>
#include "Project.h"
#include "Task.h"
#include "Employee.h"
#include "ContactInfo.h"
#include "Address.h"

TEST(ProjectTest, ConstructorTest) {
    Project project("New Project");

    EXPECT_EQ(project.getName(), "New Project");
}

TEST(ProjectTest, AddAndRemoveTaskTest) {
    Project project("New Project");
    Task task1("Task 1", "2024-11-30");
    Task task2("Task 2", "2024-12-15");

    project.addTask(&task1);
    project.addTask(&task2);
    EXPECT_EQ(project.getTasks().size(), 2);

    project.removeTask("Task 1");
    EXPECT_EQ(project.getTasks().size(), 1);
    EXPECT_EQ(project.getTasks()[0]->getDescription(), "Task 2");
}

TEST(ProjectTest, AddAndRemoveEmployeeTest) {
    Project project("New Project");
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp1(1, "John", "Doe", "Developer", 50, contact, address);
    Employee emp2(2, "Jane", "Smith", "Designer", 45, ContactInfo("098-765-4321", "jane.smith@example.com"), Address("Minsk", "Pushkina", 15));

    project.addEmployee(&emp1);
    project.addEmployee(&emp2);
    EXPECT_EQ(project.getEmployees().size(), 2);

    project.removeEmployee(1);
    EXPECT_EQ(project.getEmployees().size(), 1);
    EXPECT_EQ(project.getEmployees()[0]->getFirstName(), "Jane");
}

TEST(ProjectTest, ShowProjectDetailsTest) {
    Project project("New Project");
    Task task1("Task 1", "2024-11-30");
    Task task2("Task 2", "2024-12-15");
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp1(1, "John", "Doe", "Developer", 50, contact, address);
    Employee emp2(2, "Jane", "Smith", "Designer", 45, ContactInfo("098-765-4321", "jane.smith@example.com"), Address("Minsk", "Pushkina", 15));

    project.addTask(&task1);
    project.addTask(&task2);
    project.addEmployee(&emp1);
    project.addEmployee(&emp2);

    // Перехват стандартного вывода
    testing::internal::CaptureStdout();
    project.showProjectDetails();
    std::string output = testing::internal::GetCapturedStdout();

    std::string expectedOutput =
        "Project: New Project\n"
        "Tasks:\n"
        "- Task 1 (Due: 2024-11-30, Status: Not Completed)\n"
        "- Task 2 (Due: 2024-12-15, Status: Not Completed)\n"
        "Employees:\n"
        "- John Doe\n"
        "- Jane Smith\n";

    EXPECT_EQ(output, expectedOutput);
}

