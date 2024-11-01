project(pizzeria_lib)

set(HEADER_FILES
        BankTransferPayment.h
        CashPayment.h
        Chef.h
        CreditCardPayment.h
        Customer.h
        DeliveryPerson.h
        Employee.h
        InStoreCustomer.h
        LoyaltyProgram.h
        Manager.h
        OnlineCustomer.h
        Order.h
        Payment.h
        Person.h
        Pizza.h
)

set(SOURCE_FILES
        BankTransferPayment.cpp
        CashPayment.cpp
        Chef.cpp
        CreditCardPayment.cpp
        Customer.cpp
        DeliveryPerson.cpp
        Employee.cpp
        InStoreCustomer.cpp
        LoyaltyProgram.cpp
        Manager.cpp
        OnlineCustomer.cpp
        Order.cpp
        Payment.cpp
        Person.cpp
        Pizza.cpp
)

add_library(pizzeria_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})