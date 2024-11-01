#ifndef INSTORECUSTOMER_H
#define INSTORECUSTOMER_H

#include "Customer.h"

class InStoreCustomer : public virtual Customer {
private:
    std::string membershipCard;

public:
    InStoreCustomer(const std::string& name, int age, int customerID, int loyaltyPoints, const std::string& membershipCard);

    std::string getMembershipCard() const;
};

#endif // INSTORECUSTOMER_H
