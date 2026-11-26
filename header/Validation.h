#pragma once
#include "ValidationException.h"
#include <cmath>
#include <string>

// Вспомогательные проверки значений: при нарушении бросают ValidationException

inline void requirePositive(double value, const std::string& field) {
    if (!std::isfinite(value) || value <= 0.0)
        throw ValidationException("Поле «" + field + "» должно быть больше нуля");
}

inline void requireNotEmpty(const std::string& value, const std::string& field) {
    if (value.empty())
        throw ValidationException("Поле «" + field + "» не должно быть пустым");
}

inline void requireValidDays(int days) {
    if (days <= 0)
        throw ValidationException("Срок аренды должен быть не менее одних суток");
}
