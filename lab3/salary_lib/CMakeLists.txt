project(DateConverter)

set(HEADER_FILES
        Address.h
        Attendance.h
        BankAccount.h
        Company.h
        ContactInfo.h
        Department.h
        Employee.h
        LoginSystem.h
        Manager.h
        Payment.h
        PayrollSystem.h
        Project.h
        ProjectManager.h
        Report.h
        Role.h
        Task.h
        TimeSheet.h
)

set(SOURCE_FILES
        Address.cpp
        Attendance.cpp
        BankAccount.cpp
        Company.cpp
        ContactInfo.cpp
        Department.cpp
        Employee.cpp
        LoginSystem.cpp
        Manager.cpp
        Payment.cpp
        PayrollSystem.cpp
        Project.cpp
        ProjectManager.cpp
        Report.cpp
        Role.cpp
        Task.cpp
        TimeSheet.cpp
)

add_library(salary_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})