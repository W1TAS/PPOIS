#include "ContactInfo.h"

ContactInfo::ContactInfo(const std::string& phone, const std::string& email)
    : phone(phone), email(email) {}

void ContactInfo::updateContactInfo(const std::string& phone, const std::string& email) {
    this->phone = phone;
    this->email = email;
}

std::string ContactInfo::getPhone() const {
    return phone;
}

std::string ContactInfo::getEmail() const {
    return email;
}
