#include "RentalSystem.h"
#include <exception>
#include <iostream>

int main() {
    try {
        RentalSystem rentalSystem("equipment.txt", "customers.txt", "orders.txt");
        rentalSystem.run();
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Неизвестная критическая ошибка\n";
        return 1;
    }
    return 0;
}
