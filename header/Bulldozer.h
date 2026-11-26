#pragma once
#include "Equipment.h"

// Бульдозер: скидка 5% от 5 суток, 15% от 14 суток
class Bulldozer : public Equipment {
    double bladeWidth; // ширина отвала, м
    int enginePower;   // мощность двигателя, л.с.

public:
    Bulldozer(int equipmentId, const std::string& equipmentName, double rate, double width, int power,
              bool availability = true);

    std::string getType() const override { return "Bulldozer"; }
    std::string getSpecs() const override;
    double calculateCost(int days) const override;
    std::string serializeExtras() const override;
};
