#ifndef CASHPAYMENT_H
#define CASHPAYMENT_H

#include "Payment.h"

class CashPayment : public Payment {
private:
    double cashReceived;

public:
    CashPayment(int paymentID, double amount, double cashReceived);
    void processPayment() override;
    void refundPayment() override;
    bool wasSuccesfull() override;
};

#endif // CASHPAYMENT_H
