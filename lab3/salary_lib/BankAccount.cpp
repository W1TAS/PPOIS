#include "BankAccount.h"

BankAccount::BankAccount(const std::string& accountNumber, const std::string& bank)
    : accountNumber(accountNumber), bank(bank) {}

void BankAccount::updateBankAccount(const std::string& accountNumber, const std::string& bank) {
    this->accountNumber = accountNumber;
    this->bank = bank;
}

std::string BankAccount::getAccountNumber() const {
    return accountNumber;
}

std::string BankAccount::getBank() const {
    return bank;
}
