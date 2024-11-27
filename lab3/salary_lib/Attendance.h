#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include "Employee.h"
#include <string>

class Attendance {
public:
    Attendance(Employee* employee, const std::string& date, bool present);
    void updateAttendance(bool present);

    Employee* getEmployee() const;
    std::string getDate() const;
    bool isPresent() const;

private:
    Employee* employee;
    std::string date;
    bool present;
};

#endif // ATTENDANCE_H
