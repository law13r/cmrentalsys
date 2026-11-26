#pragma once
#include "RentalException.h"

// Ошибки работы с файлами (не открылся, не записался, не переименовался)
class FileException : public RentalException {
public:
    using RentalException::RentalException;
};
