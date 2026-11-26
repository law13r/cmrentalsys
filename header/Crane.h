#pragma once
#include "Equipment.h"

// Автокран: надбавка 15% (оператор и страховка) + фиксированная плата за мобилизацию
class Crane : public Equipment {
    double liftCapacity; // грузоподъёмность, т
    double boomLength;   // длина стрелы, м

public:
    Crane(int equipmentId, const std::string& equipmentName, double rate, double capacity, double boom,
          bool availability = true);

    std::string getType() const override { return "Crane"; }
    std::string getSpecs() const override;
    double calculateCost(int days) const override;
    std::string serializeExtras() const override;
};
