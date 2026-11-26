#pragma once
#include "DataFormatException.h"
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

// Безопасный разбор значений из строк: при ошибке бросают DataFormatException

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
