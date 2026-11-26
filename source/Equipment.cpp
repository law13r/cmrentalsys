#include "Equipment.h"
#include "DataFormatException.h"
#include "Parsing.h"
#include "Bulldozer.h"
#include "ConcreteMixer.h"
#include "Crane.h"
#include "Excavator.h"
#include "Loader.h"
#include <iomanip>
#include <sstream>

std::vector<std::string> splitString(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::string token;
    std::istringstream stream(s);
    while (std::getline(stream, token, delim)) parts.push_back(token);
    return parts;
}

std::string numToString(double value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

// ======================= Equipment =======================
Equipment::Equipment(int equipmentId, const std::string& equipmentName, double rate, bool availability)
    : id(equipmentId), name(equipmentName), dailyRate(rate), available(availability) {
    if (id <= 0) throw ValidationException("ID техники должен быть положительным");
    requireNotEmpty(name, "название");
    requirePositive(dailyRate, "тариф");
}

double Equipment::calculateCost(int days) const {
    requireValidDays(days);
    return dailyRate * days;
}

void Equipment::print(std::ostream& os) const {
    os << std::left << std::setw(4) << id
       << std::setw(15) << getType()
       << std::setw(22) << name
       << std::right << std::setw(9) << std::fixed << std::setprecision(2) << dailyRate << "/сут  "
       // оба слова по 8 символов; setw для кириллицы в UTF-8 считает байты, поэтому отступ задан вручную
       << "  " << (available ? "свободна" : "в аренде") << "  "
       << getSpecs() << "\n";
}

// Формат: Тип|id|название|тариф|доступность|доп.поля...
std::string Equipment::serialize() const {
    std::ostringstream os;
    os << getType() << '|' << id << '|' << name << '|' << dailyRate << '|'
       << (available ? 1 : 0) << '|' << serializeExtras();
    return os.str();
}

std::unique_ptr<Equipment> Equipment::deserialize(const std::string& line) {
    const auto f = splitString(line, '|');
    if (f.size() < 6)
        throw DataFormatException("Недостаточно полей в записи о технике");

    const std::string& type = f[0];
    const int itemId = parseInt(f[1], "ID");
    const std::string& itemName = f[2];
    const double itemRate = parseDouble(f[3], "тариф");
    const bool itemAvailable = parseBool(f[4], "доступность");

    if (type == "Excavator")
        return std::make_unique<Excavator>(itemId, itemName, itemRate, parseDouble(f[5], "объём ковша"), itemAvailable);

    if (type == "Crane") {
        if (f.size() < 7) throw DataFormatException("Недостаточно полей для автокрана");
        return std::make_unique<Crane>(itemId, itemName, itemRate, parseDouble(f[5], "грузоподъёмность"),
                                       parseDouble(f[6], "длина стрелы"), itemAvailable);
    }
    if (type == "Bulldozer") {
        if (f.size() < 7) throw DataFormatException("Недостаточно полей для бульдозера");
        return std::make_unique<Bulldozer>(itemId, itemName, itemRate, parseDouble(f[5], "ширина отвала"),
                                           parseInt(f[6], "мощность"), itemAvailable);
    }
    if (type == "ConcreteMixer")
        return std::make_unique<ConcreteMixer>(itemId, itemName, itemRate, parseDouble(f[5], "объём барабана"), itemAvailable);

    if (type == "Loader")
        return std::make_unique<Loader>(itemId, itemName, itemRate, parseDouble(f[5], "грузоподъёмность"), itemAvailable);

    throw DataFormatException("Неизвестный тип техники: '" + type + "'");
}
