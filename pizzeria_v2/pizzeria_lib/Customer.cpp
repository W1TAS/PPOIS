#include "Customer.h"

#include <algorithm>
#include <iostream>

Customer::Customer(const std::string& name, int age, int customerID, int loyaltyPoints)
    : Person(name, age), customerID(customerID), loyaltyPoints(loyaltyPoints) {}

int Customer::getCustomerID() const {
    return customerID;
}

int Customer::getLoyaltyPoints() const {
    return loyaltyPoints;
}

void Customer::placeOrder(Order& order) {
    std::cout << "Оформляется заказ с ID: " << order.getOrderID() << "." << std::endl;
    order.setStatus("Placed");
}

void Customer::makePayment(Payment& payment) {

    if (payment.wasSuccesfull())
    {
        payment.processPayment();
        loyaltyPoints+=payment.getAmmount();
    } else
    {
        std::cout << "Платёж не одобрен.\n";
    }
}

void Customer::useLoyaltyPoints(int points, Order& order) {
    if (points <= loyaltyPoints) {
        order.applyLoyaltyPoints(points);
        loyaltyPoints -= points;
        std::cout << points << " бонусных баллов. Новая цена: " << order.getTotalPrice() << ". Оставшиеся бонусные баллы: " << loyaltyPoints << "." << std::endl;
    } else {
        std::cout << "Неправильное значение бонусных очков" << std::endl;
    }
}

void Customer::useLoyaltyProgram(LoyaltyProgram& program, Order& order) {
    order.applyDiscount(program.getdiscountRate());
    std::cout << "Скидка применена. Новая цена: " << order.getTotalPrice() << "." << std::endl;
}
