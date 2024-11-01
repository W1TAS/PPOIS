#ifndef BANKTRANSFERPAYMENT_H
#define BANKTRANSFERPAYMENT_H

#include "Payment.h"

class BankTransferPayment : public Payment {
private:
    std::string bankAccountNumber;

public:
    BankTransferPayment(int paymentID, double amount, const std::string& bankAccountNumber);
    void processPayment() override;
    void refundPayment() override;
    bool wasSuccesfull() override;
};

#endif // BANKTRANSFERPAYMENT_H
