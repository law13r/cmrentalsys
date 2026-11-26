#pragma once
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

// ======================= Иерархия исключений =======================

// Базовое исключение приложения
class RentalException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Ошибки работы с файлами (не открылся, не записался, не переименовался)
class FileException : public RentalException {
public:
    using RentalException::RentalException;
};

// Повреждённые или некорректные данные в файле / строке
class DataFormatException : public RentalException {
public:
    using RentalException::RentalException;
};

// Некорректные значения, введённые пользователем или переданные в конструктор
class ValidationException : public RentalException {
public:
    using RentalException::RentalException;
};

// Объект не найден (техника, заказ, клиент)
class NotFoundException : public RentalException {
public:
    using RentalException::RentalException;
};

// Ошибка авторизации
class AuthException : public RentalException {
public:
    using RentalException::RentalException;
};

// Нарушение бизнес-правил (например, техника уже в аренде)
class BusinessRuleException : public RentalException {
public:
    using RentalException::RentalException;
};

// Поток ввода закрыт (Ctrl+Z / Ctrl+D, перенаправленный ввод закончился)
class InputClosedException : public RentalException {
public:
    using RentalException::RentalException;
};

// ======================= Вспомогательные проверки =======================

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

// ======================= Безопасный разбор чисел =======================

inline int parseInt(const std::string& s, const std::string& field) {
    try {
        std::size_t pos = 0;
        const int v = std::stoi(s, &pos);
        if (pos != s.size()) throw std::invalid_argument("trailing characters");
        return v;
    } catch (const std::invalid_argument&) {
        throw DataFormatException("Некорректное целое число в поле «" + field + "»: '" + s + "'");
    } catch (const std::out_of_range&) {
        throw DataFormatException("Число вне допустимого диапазона в поле «" + field + "»: '" + s + "'");
    }
}

inline double parseDouble(const std::string& s, const std::string& field) {
    try {
        std::size_t pos = 0;
        const double v = std::stod(s, &pos);
        if (pos != s.size() || !std::isfinite(v)) throw std::invalid_argument("bad number");
        return v;
    } catch (const std::invalid_argument&) {
        throw DataFormatException("Некорректное число в поле «" + field + "»: '" + s + "'");
    } catch (const std::out_of_range&) {
        throw DataFormatException("Число вне допустимого диапазона в поле «" + field + "»: '" + s + "'");
    }
}

// В файлах логические значения хранятся как "0" или "1"
inline bool parseBool(const std::string& s, const std::string& field) {
    if (s == "1") return true;
    if (s == "0") return false;
    throw DataFormatException("Поле «" + field + "» должно быть 0 или 1, получено: '" + s + "'");
}
