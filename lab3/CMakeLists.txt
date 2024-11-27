cmake_minimum_required(VERSION 3.29)
project(lab3)

set(CMAKE_CXX_STANDARD 20)


add_executable(lab3 main.cpp
        salary_lib/Employee.cpp
        salary_lib/Employee.h
        salary_lib/Manager.cpp
        salary_lib/Manager.h
        salary_lib/Department.cpp
        salary_lib/Department.h
        salary_lib/Company.cpp
        salary_lib/Company.h
        salary_lib/PayrollSystem.cpp
        salary_lib/PayrollSystem.h
        salary_lib/TimeSheet.cpp
        salary_lib/TimeSheet.h
        salary_lib/Payment.cpp
        salary_lib/Payment.h
        salary_lib/LoginSystem.cpp
        salary_lib/LoginSystem.h
        salary_lib/Address.cpp
        salary_lib/Address.h
        salary_lib/Project.cpp
        salary_lib/Project.h
        salary_lib/ProjectManager.cpp
        salary_lib/ProjectManager.h
        salary_lib/Task.cpp
        salary_lib/Task.h
        salary_lib/ContactInfo.cpp
        salary_lib/ContactInfo.h
        salary_lib/Report.cpp
        salary_lib/Report.h
        salary_lib/BankAccount.cpp
        salary_lib/BankAccount.h
        salary_lib/Attendance.cpp
        salary_lib/Attendance.h
        salary_lib/Role.cpp
        salary_lib/Role.h
        Google_tests/Address_TEST.cpp
)

include_directories(salary_lib)
add_subdirectory(salary_lib)

target_link_libraries(lab3 salary_lib)

add_subdirectory(Google_tests)