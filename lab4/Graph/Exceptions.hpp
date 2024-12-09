#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class GraphException : public std::runtime_error {
public:
    explicit GraphException(const std::string& message)
        : std::runtime_error("Graph Exception: " + message) {}
};

#endif // EXCEPTIONS_HPP
