#ifndef CREDITCARDPAYMENT_H
#define CREDITCARDPAYMENT_H

#include "Payment.h"

class CreditCardPayment : public Payment {
private:
    std::string cardNumber;
    std::string expiryDate;
    double balance;

public:
    CreditCardPayment(int paymentID, double amount, const std::string& cardNumber, const std::string& expiryDate, double balance);
    void processPayment() override;
    void refundPayment() override;
    bool wasSuccesfull() override;
    double getBalance();
};

#endif // CREDITCARDPAYMENT_H
