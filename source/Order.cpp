#include "Order.h"
#include "DataFormatException.h"
#include "Equipment.h" // splitString
#include "Parsing.h"
#include <sstream>

std::string Order::serialize() const {
    std::ostringstream os;
    os << id << '|' << customerId << '|' << equipmentId << '|' << days << '|'
       << totalCost << '|' << startDate << '|' << (returned ? 1 : 0);
    return os.str();
}

Order Order::deserialize(const std::string& line) {
    const auto f = splitString(line, '|');
    if (f.size() < 7)
        throw DataFormatException("Недостаточно полей в записи о заказе");

    Order o;
    o.id = parseInt(f[0], "ID заказа");
    o.customerId = parseInt(f[1], "ID клиента");
    o.equipmentId = parseInt(f[2], "ID техники");
    o.days = parseInt(f[3], "срок аренды");
    o.totalCost = parseDouble(f[4], "стоимость");
    o.startDate = f[5];
    o.returned = parseBool(f[6], "возвращена");

    if (o.id <= 0 || o.customerId <= 0 || o.equipmentId <= 0)
        throw DataFormatException("Идентификаторы в заказе должны быть положительными");
    if (o.days <= 0)
        throw DataFormatException("Срок аренды в заказе должен быть положительным");
    if (o.totalCost < 0)
        throw DataFormatException("Стоимость заказа не может быть отрицательной");
    if (o.startDate.empty())
        throw DataFormatException("В заказе не указана дата начала аренды");
    return o;
}
