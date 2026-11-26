#pragma once
#include <stdexcept>

// Базовое исключение приложения: от него наследуются все остальные
class RentalException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
