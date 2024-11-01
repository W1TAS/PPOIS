#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>

class Payment {
protected:
    int paymentID;
    double amount;

public:
    Payment(int paymentID, double amount);
    virtual void processPayment() = 0;
    virtual void refundPayment() = 0;
    void setAmount(double newAmount);
    double getAmmount();
    virtual bool wasSuccesfull() = 0;
};

#endif // PAYMENT_H
