#include "TimeSheet.h"

TimeSheet::TimeSheet(Employee* employee, const std::string& date, int hours)
    : employee(employee), date(date), hoursWorked(hours) {}

void TimeSheet::fillTimeSheet(int hours) {
    hoursWorked = hours;
}

Employee* TimeSheet::getEmployee() const { return employee; }
std::string TimeSheet::getDate() const { return date; }
int TimeSheet::getHoursWorked() const { return hoursWorked; }
