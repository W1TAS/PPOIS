#ifndef ONLINECUSTOMER_H
#define ONLINECUSTOMER_H

#include "Customer.h"

class OnlineCustomer : public virtual Customer {
private:
    std::string email;
    std::string password;

public:
    OnlineCustomer(const std::string& name, int age, int customerID, int loyaltyPoints, const std::string& email, const std::string& password);

    std::string getEmail() const;
    bool checkPassword(const std::string& password) const;
    void resetPassword(const std::string& newPassword);
};

#endif // ONLINECUSTOMER_H
