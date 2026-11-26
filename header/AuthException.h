#pragma once
#include "RentalException.h"

// Ошибка авторизации (неверный логин или пароль)
class AuthException : public RentalException {
public:
    using RentalException::RentalException;
};
