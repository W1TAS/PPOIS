#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

#include <string>

class BankAccount {
public:
    BankAccount(const std::string& accountNumber, const std::string& bank);
    void updateBankAccount(const std::string& accountNumber, const std::string& bank);

    std::string getAccountNumber() const;
    std::string getBank() const;

private:
    std::string accountNumber;
    std::string bank;
};

#endif // BANKACCOUNT_H
