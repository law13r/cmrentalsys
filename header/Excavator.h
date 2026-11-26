#pragma once
#include "Equipment.h"

// Экскаватор: скидка 10% при аренде от 7 суток
class Excavator : public Equipment {
    double bucketVolume; // объём ковша, м3

public:
    Excavator(int equipmentId, const std::string& equipmentName, double rate, double volume, bool availability = true);

    std::string getType() const override { return "Excavator"; }
    std::string getSpecs() const override;
    double calculateCost(int days) const override;
    std::string serializeExtras() const override;

    double getBucketVolume() const { return bucketVolume; }
};
