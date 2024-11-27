#ifndef TIMESHEET_H
#define TIMESHEET_H

#include "Employee.h"
#include <string>

class TimeSheet {
public:
    TimeSheet(Employee* employee, const std::string& date, int hours);
    void fillTimeSheet(int hours);

    Employee* getEmployee() const;
    std::string getDate() const;
    int getHoursWorked() const;

private:
    Employee* employee;
    std::string date;
    int hoursWorked;
};

#endif // TIMESHEET_H
