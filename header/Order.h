#pragma once
#include <string>

// Запись о заказе (аренде)
struct Order {
    int id = 0;
    int customerId = 0;
    int equipmentId = 0;
    int days = 0;
    double totalCost = 0.0;
    std::string startDate{};
    bool returned = false;

    // Формат строки: id|customerId|equipmentId|days|totalCost|startDate|returned
    std::string serialize() const;
    // Бросает DataFormatException, если строка повреждена
    static Order deserialize(const std::string& line);
};
