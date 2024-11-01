#include "Payment.h"

Payment::Payment(int paymentID, double amount)
    : paymentID(paymentID), amount(amount) {}

void Payment::setAmount(double newAmount) {
    amount = newAmount;
}

double Payment::getAmmount()
{
    return amount;
}

