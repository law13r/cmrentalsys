#pragma once
#include "RentalException.h"

// Нарушение бизнес-правил (например, техника уже в аренде)
class BusinessRuleException : public RentalException {
public:
    using RentalException::RentalException;
};
