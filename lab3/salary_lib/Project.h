#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <vector>
#include "Employee.h"
#include "Task.h"

class Project {
public:
    Project(const std::string& name);
    void addTask(Task* task);
    void addEmployee(Employee* employee);
    void removeTask(const std::string& description);
    void removeEmployee(int id);
    void showProjectDetails() const;
    std::string getName() const;
    std::vector<Task*> getTasks();
    std::vector<Employee*> getEmployees();

private:
    std::string name;
    std::vector<Task*> tasks;
    std::vector<Employee*> employees;
};

#endif // PROJECT_H
