#include "LoyaltyProgram.h"

LoyaltyProgram::LoyaltyProgram(int programID, double discountRate)
    : programID(programID), discountRate(discountRate) {}

void LoyaltyProgram::applyDiscount(double& price) const {
    price -= price * discountRate;
}

double LoyaltyProgram::getdiscountRate()
{
    return this->discountRate;
}
