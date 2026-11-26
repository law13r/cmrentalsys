#pragma once
#include "Equipment.h"

// Погрузчик: скидка 10% от 10 суток
class Loader : public Equipment {
    double loadCapacity; // грузоподъёмность, т

public:
    Loader(int equipmentId, const std::string& equipmentName, double rate, double capacity, bool availability = true);

    std::string getType() const override { return "Loader"; }
    std::string getSpecs() const override;
    double calculateCost(int days) const override;
    std::string serializeExtras() const override;
};
