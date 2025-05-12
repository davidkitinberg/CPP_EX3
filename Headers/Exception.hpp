#pragma once
#include <stdexcept>

class IllegalActionException : public std::runtime_error {
public:
    IllegalActionException(const std::string& message) : std::runtime_error(message) {}
};
