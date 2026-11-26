#include "Loader.h"

Loader::Loader(int equipmentId, const std::string& equipmentName, double rate, double capacity, bool availability)
    : Equipment(equipmentId, equipmentName, rate, availability), loadCapacity(capacity) {
    requirePositive(loadCapacity, "грузоподъёмность");
}

std::string Loader::getSpecs() const {
    return "грузоподъёмность " + numToString(loadCapacity) + " т";
}

double Loader::calculateCost(int days) const {
    requireValidDays(days);
    double cost = dailyRate * days;
    if (days >= 10) cost *= 0.90;
    return cost;
}

std::string Loader::serializeExtras() const {
    return numToString(loadCapacity);
}
