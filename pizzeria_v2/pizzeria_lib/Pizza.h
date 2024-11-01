#ifndef PIZZA_H
#define PIZZA_H

#include <string>
#include <vector>

class Pizza {
private:
    std::string size;
    std::string crustType;
    std::vector<std::string> toppings;
    double price;

public:
    Pizza(const std::string& size, const std::string& crustType, double price);

    std::string getSize() const;
    std::string getCrustType() const;
    std::vector<std::string> getToppings() const;
    double getPrice() const;
    void addTopping(const std::string& topping);
    bool operator==(const Pizza& other) const;
};

#endif // PIZZA_H
