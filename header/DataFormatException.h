#pragma once
#include "RentalException.h"

// Повреждённые или некорректные данные в файле / строке
class DataFormatException : public RentalException {
public:
    using RentalException::RentalException;
};
