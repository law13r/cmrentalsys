#pragma once
#include "Equipment.h"

// Бетономешалка (автобетоносмеситель): фиксированная плата за мойку барабана
class ConcreteMixer : public Equipment {
    double drumVolume; // объём барабана, м3

public:
    ConcreteMixer(int equipmentId, const std::string& equipmentName, double rate, double volume, bool availability = true);

    std::string getType() const override { return "ConcreteMixer"; }
    std::string getSpecs() const override;
    double calculateCost(int days) const override;
    std::string serializeExtras() const override;
};
