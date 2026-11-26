#pragma once
#include "Validation.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ======================= Базовый класс =======================
class Equipment {
protected:
    int id;
    std::string name;
    double dailyRate;   // базовая стоимость аренды в сутки
    bool available;     // свободна ли техника

public:
    // Бросает ValidationException при некорректных параметрах
    Equipment(int equipmentId, const std::string& equipmentName, double rate, bool availability = true);
    virtual ~Equipment() = default;

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    double getDailyRate() const { return dailyRate; }
    bool isAvailable() const { return available; }
    void setAvailable(bool value) { available = value; }

    // Полиморфные методы
    virtual std::string getType() const = 0;                 // название типа техники
    virtual std::string getSpecs() const = 0;                // характеристики для вывода
    virtual double calculateCost(int days) const;            // стоимость аренды (у каждого типа своя формула)
    virtual std::string serializeExtras() const = 0;         // специфичные поля для файла

    void print(std::ostream& os) const;
    std::string serialize() const;                           // строка для записи в файл

    // Фабрика: создаёт нужный объект по строке из файла.
    // Бросает DataFormatException (или ValidationException), если строка повреждена.
    static std::unique_ptr<Equipment> deserialize(const std::string& line);
};

// Вспомогательные функции
std::vector<std::string> splitString(const std::string& s, char delim);
std::string numToString(double value);
