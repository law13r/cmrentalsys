#pragma once
#include "RentalException.h"

// Поток ввода закрыт (Ctrl+Z / Ctrl+D, перенаправленный ввод закончился)
class InputClosedException : public RentalException {
public:
    using RentalException::RentalException;
};
