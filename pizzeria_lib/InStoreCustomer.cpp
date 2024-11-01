#include "InStoreCustomer.h"
#include <iostream>

InStoreCustomer::InStoreCustomer(const std::string& name, int age, int customerID, int loyaltyPoints, const std::string& membershipCard)
    : Customer(name, age, customerID, loyaltyPoints), membershipCard(membershipCard) {}

std::string InStoreCustomer::getMembershipCard() const {
    return membershipCard;
}

