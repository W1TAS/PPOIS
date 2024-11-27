#include "Attendance.h"

Attendance::Attendance(Employee* employee, const std::string& date, bool present)
    : employee(employee), date(date), present(present) {}

void Attendance::updateAttendance(bool present) {
    this->present = present;
}

Employee* Attendance::getEmployee() const { return employee; }
std::string Attendance::getDate() const { return date; }
bool Attendance::isPresent() const { return present; }
