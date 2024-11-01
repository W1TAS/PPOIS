#include "Order.h"
#include <algorithm>

Order::Order(int orderID, Customer* customer)
    : orderID(orderID), customer(customer), totalPrice(0.0), status("Pending") {}

int Order::getOrderID() const {
    return orderID;
}

Customer* Order::getCustomer() const {
    return customer;
}

std::vector<Pizza> Order::getPizzas() const {
    return pizzas;
}

void Order::addPizza(const Pizza& pizza) {
    pizzas.push_back(pizza);
    calculateTotalPrice();
}

void Order::calculateTotalPrice() {
    totalPrice = 0.0;
    for (const auto& pizza : pizzas) {
        totalPrice += pizza.getPrice();
    }
}

std::string Order::getStatus() const {
    return status;
}

void Order::setStatus(const std::string& status) {
    this->status = status;
}

void Order::removePizza(const Pizza& pizza) {
    auto it = std::find(pizzas.begin(), pizzas.end(), pizza);
    if (it != pizzas.end()) {
        pizzas.erase(it);
        calculateTotalPrice();
    }
}

void Order::updatePizza(const Pizza& oldPizza, const Pizza& newPizza) {
    auto it = std::find(pizzas.begin(), pizzas.end(), oldPizza);
    if (it != pizzas.end()) {
        *it = newPizza;
        calculateTotalPrice();
    }
}

void Order::applyDiscount(double discountRate) {
    totalPrice -= totalPrice * discountRate;
}

int Order::applyLoyaltyPoints(int points) {
    int pointsApplied = points;

    if (0.05*points >= totalPrice) {
        pointsApplied = totalPrice*20;
        totalPrice = 0;
    } else {
        totalPrice -= 0.05 * points;
    }

    return pointsApplied;
}

double Order::getTotalPrice() const {
    return totalPrice;
}
