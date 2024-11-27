#include "LoginSystem.h"
#include <stdexcept> // Включение заголовка

LoginSystem::LoginSystem() {}

bool LoginSystem::login(const std::string& username, const std::string& password) {
    auto it = userDatabase.find(username);
    if (it != userDatabase.end() && it->second == password) {
        return true;
    }
    return false;
}

void LoginSystem::registerUser(const std::string& username, const std::string& password) {
    if (userDatabase.find(username) != userDatabase.end()) {
        throw std::invalid_argument("User with this login already exists");
    }
    userDatabase[username] = password;
}
