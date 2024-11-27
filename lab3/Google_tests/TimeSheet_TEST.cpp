#include <gtest/gtest.h>
#include "TimeSheet.h"
#include "Employee.h"
#include "ContactInfo.h"
#include "Address.h"

TEST(TimeSheetTest, ConstructorTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);
    TimeSheet timesheet(&emp, "2024-11-25", 160);

    EXPECT_EQ(timesheet.getEmployee()->getFirstName(), "John");
    EXPECT_EQ(timesheet.getDate(), "2024-11-25");
    EXPECT_EQ(timesheet.getHoursWorked(), 160);
}

TEST(TimeSheetTest, FillTimeSheetTest) {
    ContactInfo contact("123-456-7890", "john.doe@example.com");
    Address address("Minsk", "Lenina", 10);
    Employee emp(1, "John", "Doe", "Developer", 50, contact, address);
    TimeSheet timesheet(&emp, "2024-11-25", 160);

    timesheet.fillTimeSheet(200);
    EXPECT_EQ(timesheet.getHoursWorked(), 200);
}


