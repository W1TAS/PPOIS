project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_executable(Google_test_run PizzaTest.cpp
        BankTransferTest.cpp
        CashPaymentTest.cpp
        ChefTest.cpp
        CreditCardPaymentTest.cpp
        CustomerTest.cpp
        DelivertPersonTest.cpp
        InStoreCustomerTest.cpp
        LoyaltyProgramTest.cpp
        ManagerTest.cpp
        OnlineCustomerTest.cpp
        OrderTest.cpp
        PaymentTest.cpp
        EmployeeTest.cpp
        PersonTest.cpp
)
target_link_libraries(Google_test_run pizzeria_lib)
target_link_libraries(Google_test_run gtest gtest_main)