#include "BankTransferPayment.h"

#include <iostream>
#include <regex>

BankTransferPayment::BankTransferPayment(int paymentID, double amount, const std::string& bankAccountNumber)
    : Payment(paymentID, amount), bankAccountNumber(bankAccountNumber) {}

void BankTransferPayment::processPayment() {
    if (wasSuccesfull())
    {
        std::cout << "Платеж на сумму " << amount << " рублей успешно переведен на счет " << bankAccountNumber << "." << std::endl;
    } else
    {
        std::cout << "Ошибка: недействительный банковский номер счета.\n";
    }
}

void BankTransferPayment::refundPayment() {
    if (wasSuccesfull())
    {
        std::cout << "Возврат платежа на сумму " << amount << " рублей успешно произведен на счет " << bankAccountNumber << "." << std::endl;
    } else
    {
        std::cout << "Ошибка: недействительный банковский номер счета.\n";
    }
}
bool BankTransferPayment::wasSuccesfull()
{
    std::regex accountNumberPattern("\\d{10,20}");
    return std::regex_match(bankAccountNumber,accountNumberPattern);
}
