#ifndef ROLE_H
#define ROLE_H

#include <string>

class Role {
public:
    Role(const std::string& name, const std::string& permissions);
    void updateRole(const std::string& name, const std::string& permissions);

    std::string getName() const;
    std::string getPermissions() const;

private:
    std::string name;
    std::string permissions;
};

#endif // ROLE_H
