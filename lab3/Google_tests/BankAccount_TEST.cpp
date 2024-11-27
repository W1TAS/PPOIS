#include <gtest/gtest.h>
#include "BankAccount.h"

TEST(BankAccountTest, ConstructorTest) {
  BankAccount account("123456789", "Tech Bank");

  EXPECT_EQ(account.getAccountNumber(), "123456789");
  EXPECT_EQ(account.getBank(), "Tech Bank");
}

TEST(BankAccountTest, UpdateBankAccountTest) {
  BankAccount account("123456789", "Tech Bank");
  account.updateBankAccount("987654321", "Finance Bank");

  EXPECT_EQ(account.getAccountNumber(), "987654321");
  EXPECT_EQ(account.getBank(), "Finance Bank");
}



