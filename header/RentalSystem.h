#pragma once
#include "Customer.h"
#include "Equipment.h"
#include "Order.h"
#include <memory>
#include <string>
#include <vector>

class RentalSystem {
    std::vector<std::unique_ptr<Equipment>> equipment{};
    std::vector<Customer> customers{};
    std::vector<Order> orders{};
    Customer* current = nullptr; // авторизованный пользователь

    std::string equipmentFile;
    std::string customersFile;
    std::string ordersFile;

    // Работа с файлами (FileException при ошибках ввода-вывода;
    // повреждённые строки пропускаются с предупреждением)
    void loadEquipment();
    void loadCustomers();
    void loadOrders();
    void saveEquipment() const;
    void saveCustomers() const;
    void saveOrders() const;

    // Поиск
    Equipment* findEquipment(int id);
    Equipment& requireEquipment(int id);               // NotFoundException
    Customer* findCustomerByLogin(const std::string& loginName);
    Customer* findCustomerById(int id);
    int nextEquipmentId() const;
    int nextCustomerId() const;
    int nextOrderId() const;

    // Операции (при ошибках бросают исключения, обрабатываются в run())
    void registerCustomer();
    void login();
    void showEquipment(bool onlyAvailable) const;
    void rentEquipment();
    void returnEquipment();
    void showMyOrders() const;
    void showAllOrders() const;
    void addEquipment();
    void removeEquipment();
    void showCustomers() const;

    // Меню
    bool guestMenu();   // false — пользователь выбрал выход
    void customerMenu();
    void adminMenu();

    // Ввод (InputClosedException, если поток ввода закрыт)
    static std::string readLine(const std::string& prompt);
    static int readInt(const std::string& prompt, int minV, int maxV);
    static double readDouble(const std::string& prompt, double minV);
    static std::string currentDate();

public:
    RentalSystem(const std::string& equipmentPath = "equipment.txt",
                 const std::string& customersPath = "customers.txt",
                 const std::string& ordersPath = "orders.txt");

    // Система хранит указатель на текущего пользователя, поэтому копирование запрещено
    RentalSystem(const RentalSystem&) = delete;
    RentalSystem& operator=(const RentalSystem&) = delete;

    void loadAll();
    void saveAll() const;
    void run(); // главный цикл программы
};
