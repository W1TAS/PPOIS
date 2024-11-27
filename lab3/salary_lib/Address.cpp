#include "Address.h"

Address::Address(const std::string& city, const std::string& street, int houseNumber)
    : city(city), street(street), houseNumber(houseNumber) {}

void Address::updateAddress(const std::string& city, const std::string& street, int houseNumber) {
    this->city = city;
    this->street = street;
    this->houseNumber = houseNumber;
}

std::string Address::getCity() const { return city; }
std::string Address::getStreet() const { return street; }
int Address::getHouseNumber() const { return houseNumber; }
