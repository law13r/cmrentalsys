#include "Excavator.h"

Excavator::Excavator(int equipmentId, const std::string& equipmentName, double rate, double volume, bool availability)
    : Equipment(equipmentId, equipmentName, rate, availability), bucketVolume(volume) {
    requirePositive(bucketVolume, "объём ковша");
}

std::string Excavator::getSpecs() const {
    return "ковш " + numToString(bucketVolume) + " м3";
}

double Excavator::calculateCost(int days) const {
    requireValidDays(days);
    double cost = dailyRate * days;
    if (days >= 7) cost *= 0.90;
    return cost;
}

std::string Excavator::serializeExtras() const {
    return numToString(bucketVolume);
}
