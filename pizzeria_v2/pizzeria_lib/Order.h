#ifndef ORDER_H
#define ORDER_H

#include <vector>
#include <string>
#include "Pizza.h"
#include "Customer.h"

class Customer;

class Order {
private:
    int orderID;
    Customer* customer;
    std::vector<Pizza> pizzas;
    double totalPrice;
    std::string status;

public:
    Order(int orderID, Customer* customer);

    int getOrderID() const;
    Customer* getCustomer() const;
    std::vector<Pizza> getPizzas() const;
    void addPizza(const Pizza& pizza);
    void calculateTotalPrice();
    std::string getStatus() const;
    void setStatus(const std::string& status);
    void removePizza(const Pizza& pizza);
    void updatePizza(const Pizza& oldPizza, const Pizza& newPizza);
    void applyDiscount(double discountRate);
    int applyLoyaltyPoints(int points);
    double getTotalPrice() const;
};

#endif // ORDER_H
