#pragma once
#include "RentalException.h"

// Объект не найден (техника, заказ, клиент)
class NotFoundException : public RentalException {
public:
    using RentalException::RentalException;
};
