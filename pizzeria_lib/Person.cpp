#include "Person.h"

Person::Person(const std::string& name, int age) : name(name), age(age) {}

std::string Person::getName() const {
    return name;
}

int Person::getAge() const {
    return age;
}
