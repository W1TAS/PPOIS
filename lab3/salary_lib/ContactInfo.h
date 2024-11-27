#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <string>

class ContactInfo {
public:
    ContactInfo(const std::string& phone, const std::string& email);
    void updateContactInfo(const std::string& phone, const std::string& email);

    std::string getPhone() const;
    std::string getEmail() const;

private:
    std::string phone;
    std::string email;
};

#endif // CONTACTINFO_H
