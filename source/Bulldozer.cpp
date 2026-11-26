#include "Bulldozer.h"

Bulldozer::Bulldozer(int equipmentId, const std::string& equipmentName, double rate, double width, int power,
                     bool availability)
    : Equipment(equipmentId, equipmentName, rate, availability), bladeWidth(width), enginePower(power) {
    requirePositive(bladeWidth, "ширина отвала");
    requirePositive(enginePower, "мощность");
}

std::string Bulldozer::getSpecs() const {
    return "отвал " + numToString(bladeWidth) + " м, " + std::to_string(enginePower) + " л.с.";
}

double Bulldozer::calculateCost(int days) const {
    requireValidDays(days);
    double cost = dailyRate * days;
    if (days >= 14) cost *= 0.85;
    else if (days >= 5) cost *= 0.95;
    return cost;
}

std::string Bulldozer::serializeExtras() const {
    return numToString(bladeWidth) + "|" + std::to_string(enginePower);
}
