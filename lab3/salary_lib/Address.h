#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>

class Address {
public:
    Address(const std::string& city, const std::string& street, int houseNumber);
    void updateAddress(const std::string& city, const std::string& street, int houseNumber);

    std::string getCity() const;
    std::string getStreet() const;
    int getHouseNumber() const;

private:
    std::string city;
    std::string street;
    int houseNumber;
};

#endif // ADDRESS_H
