project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# adding the Google_Tests_run target
add_executable(Google_Tests_run Address_TEST.cpp
        Attendance_TEST.cpp
        BankAccount_TEST.cpp
        Company_TEST.cpp
        ContactInfo_TEST.cpp
        Department_TEST.cpp
        Employee_TEST.cpp
        LoginSystem_TEST.cpp
        Payment_TEST.cpp
        PayrollSystem_TEST.cpp
        Project_TEST.cpp
        ProjectManager_TEST.cpp
        Report_TEST.cpp
        Role_TEST.cpp
        Task_TEST.cpp
        TimeSheet_TEST.cpp)

# linking Google_Tests_run with DateConverter_lib which will be tested
target_link_libraries(Google_Tests_run salary_lib)

target_link_libraries(Google_Tests_run gtest gtest_main)