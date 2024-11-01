#include "Pizza.h"

Pizza::Pizza(const std::string& size, const std::string& crustType, double price)
    : size(size), crustType(crustType), price(price) {}

std::string Pizza::getSize() const {
    return size;
}

std::string Pizza::getCrustType() const {
    return crustType;
}

std::vector<std::string> Pizza::getToppings() const {
    return toppings;
}

double Pizza::getPrice() const {
    return price;
}

void Pizza::addTopping(const std::string& topping) {
    toppings.push_back(topping);
}

bool Pizza::operator==(const Pizza& other) const {
    return size == other.size && crustType == other.crustType && toppings == other.toppings && price == other.price;
}
