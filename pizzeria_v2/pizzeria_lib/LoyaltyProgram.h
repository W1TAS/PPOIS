#ifndef LOYALTYPROGRAM_H
#define LOYALTYPROGRAM_H

class LoyaltyProgram {
private:
    int programID;
    double discountRate;

public:
    LoyaltyProgram(int programID, double discountRate);
    void applyDiscount(double& price) const;
    double getdiscountRate();
};

#endif // LOYALTYPROGRAM_H
