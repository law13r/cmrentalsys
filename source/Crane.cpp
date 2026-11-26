#include "Crane.h"

Crane::Crane(int equipmentId, const std::string& equipmentName, double rate, double capacity, double boom,
             bool availability)
    : Equipment(equipmentId, equipmentName, rate, availability), liftCapacity(capacity), boomLength(boom) {
    requirePositive(liftCapacity, "грузоподъёмность");
    requirePositive(boomLength, "длина стрелы");
}

std::string Crane::getSpecs() const {
    return numToString(liftCapacity) + " т, стрела " + numToString(boomLength) + " м";
}

double Crane::calculateCost(int days) const {
    requireValidDays(days);
    const double mobilizationFee = 200.0;
    return dailyRate * days * 1.15 + mobilizationFee;
}

std::string Crane::serializeExtras() const {
    return numToString(liftCapacity) + "|" + numToString(boomLength);
}
