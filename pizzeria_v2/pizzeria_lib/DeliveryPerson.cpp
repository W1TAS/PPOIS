#include "DeliveryPerson.h"
#include <iostream>

DeliveryPerson::DeliveryPerson(const std::string& name, int age, int employeeID, double salary, const std::string& vehicleType)
    : Employee(name, age, employeeID, salary), vehicleType(vehicleType) {}

std::string DeliveryPerson::getVehicleType() const {
    return vehicleType;
}

void DeliveryPerson::deliverOrder(Order& order) {
    std::cout << "Доставляем заказ с ID: " << order.getOrderID() << " используя " << vehicleType << "." << std::endl;
    order.setStatus("Delivered");
}

void DeliveryPerson::trackOrder(const std::string& orderID) {
    std::cout << "Отслеживаем заказ с ID: " << orderID << "." << std::endl;
}

double DeliveryPerson::calculateBonus() const {
    return salary * 0.05;
}

void DeliveryPerson::work() {
    std::cout << "Курьер работает над доставкой заказов" << std::endl;
}
