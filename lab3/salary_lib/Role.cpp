#include "Role.h"

Role::Role(const std::string& name, const std::string& permissions)
    : name(name), permissions(permissions) {}

void Role::updateRole(const std::string& name, const std::string& permissions) {
    this->name = name;
    this->permissions = permissions;
}

std::string Role::getName() const {
    return name;
}

std::string Role::getPermissions() const {
    return permissions;
}
