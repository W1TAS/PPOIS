#include "Project.h"
#include <iostream>
#include <algorithm>

Project::Project(const std::string& name) : name(name) {}

void Project::addTask(Task* task) {
    tasks.push_back(task);
}

void Project::addEmployee(Employee* employee) {
    employees.push_back(employee);
}

void Project::removeTask(const std::string& description) {
    tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
               [description](Task* task) { return task->getDescription() == description; }), tasks.end());
}

void Project::removeEmployee(int id) {
    employees.erase(std::remove_if(employees.begin(), employees.end(),
                  [id](Employee* employee) { return employee->getID() == id; }), employees.end());
}

void Project::showProjectDetails() const {
    std::cout << "Project: " << name << std::endl;
    std::cout << "Tasks:" << std::endl;
    for (Task* task : tasks) {
        std::cout << "- " << task->getDescription() << " (Due: " << task->getDeadline() << ", Status: " << (task->isCompleted() ? "Completed" : "Not Completed") << ")" << std::endl;
    }
    std::cout << "Employees:" << std::endl;
    for (Employee* employee : employees) {
        std::cout << "- " << employee->getFirstName() << " " << employee->getLastName() << std::endl;
    }
}

std::string Project::getName() const
{
    return name;
}

std::vector<Task*> Project::getTasks()
{
    return tasks;
}

std::vector<Employee*> Project::getEmployees()
{
    return employees;
}