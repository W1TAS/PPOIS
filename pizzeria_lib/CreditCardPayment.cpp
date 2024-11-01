#include "CreditCardPayment.h"
#include <iostream>

CreditCardPayment::CreditCardPayment(int paymentID, double amount, const std::string& cardNumber, const std::string& expiryDate, double balance)
    : Payment(paymentID, amount), cardNumber(cardNumber), expiryDate(expiryDate), balance(balance) {}

void CreditCardPayment::processPayment() {
    if (wasSuccesfull()) {
        std::cout << "Платеж на сумму " << amount << " рублей успешно обработан." << std::endl;
        balance -= amount;
    } else {
        std::cout << "Платеж отклонен." << std::endl;
    }
}

void CreditCardPayment::refundPayment() {
    if ((cardNumber.length() == 16 && expiryDate.length() == 5)) {
        std::cout << "Возврат платежа на сумму " << amount << " рублей успешно произведен." << std::endl;
        balance+=amount;
    } else {
        std::cout << "Возврат отклонен из-за ошибки валидации карты." << std::endl;
    }
}

bool CreditCardPayment::wasSuccesfull()
{
    return  ((cardNumber.length() == 16 && expiryDate.length() == 5) and balance>= amount);
}

double CreditCardPayment::getBalance()
{
    return balance;
}
