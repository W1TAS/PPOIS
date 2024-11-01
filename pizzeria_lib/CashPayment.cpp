#include "CashPayment.h"
#include <iostream>

CashPayment::CashPayment(int paymentID, double amount, double cashReceived)
    : Payment(paymentID, amount), cashReceived(cashReceived) {}

void CashPayment::processPayment() {
    if (wasSuccesfull()) {
        std::cout << "Платеж успешно обработан. Сдача: " << (cashReceived - amount) << " рублей." << std::endl;
    } else {
        std::cout << "Недостаточно наличных для оплаты." << std::endl;
    }
}

void CashPayment::refundPayment() {
    std::cout << "Возврат платежа в размере " << amount << " рублей успешно произведен." << std::endl;
}

bool CashPayment::wasSuccesfull()
{
return (cashReceived >= amount);
}

