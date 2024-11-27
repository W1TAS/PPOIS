#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H

#include <string>
#include <unordered_map>

class LoginSystem {
public:
    LoginSystem();
    bool login(const std::string& username, const std::string& password);
    void registerUser(const std::string& username, const std::string& password);

private:
    std::unordered_map<std::string, std::string> userDatabase;
};

#endif // LOGINSYSTEM_H
