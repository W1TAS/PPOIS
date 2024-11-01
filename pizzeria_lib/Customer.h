#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Person.h"
#include "Order.h"
#include "Payment.h"
#include "LoyaltyProgram.h"

class Order;

class Customer : public Person {
protected:
    int customerID;
    int loyaltyPoints;

public:
    Customer(const std::string& name, int age, int customerID, int loyaltyPoints);

    int getCustomerID() const;
    int getLoyaltyPoints() const;
    void placeOrder(Order& order);

    void makePayment(Payment& payment);

    void useLoyaltyPoints(int points, Order& order);
    void useLoyaltyProgram(LoyaltyProgram& program, Order& order);
};

#endif // CUSTOMER_H
