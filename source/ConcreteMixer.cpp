#include "ConcreteMixer.h"

ConcreteMixer::ConcreteMixer(int equipmentId, const std::string& equipmentName, double rate, double volume, bool availability)
    : Equipment(equipmentId, equipmentName, rate, availability), drumVolume(volume) {
    requirePositive(drumVolume, "объём барабана");
}

std::string ConcreteMixer::getSpecs() const {
    return "барабан " + numToString(drumVolume) + " м3";
}

double ConcreteMixer::calculateCost(int days) const {
    requireValidDays(days);
    const double washFee = 30.0;
    return dailyRate * days + washFee;
}

std::string ConcreteMixer::serializeExtras() const {
    return numToString(drumVolume);
}
