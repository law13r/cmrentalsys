#pragma once
#include "RentalException.h"

// Некорректные значения, введённые пользователем или переданные в конструктор
class ValidationException : public RentalException {
public:
    using RentalException::RentalException;
};
