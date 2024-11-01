#include "OnlineCustomer.h"
#include <iostream>

OnlineCustomer::OnlineCustomer(const std::string& name, int age, int customerID, int loyaltyPoints, const std::string& email, const std::string& password)
    : Customer(name, age, customerID, loyaltyPoints), email(email), password(password) {}

std::string OnlineCustomer::getEmail() const {
    return email;
}

bool OnlineCustomer::checkPassword(const std::string& password) const {
    return this->password == password;
}

void OnlineCustomer::resetPassword(const std::string& newPassword) {
    password = newPassword;
}
