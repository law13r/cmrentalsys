#include "RentalSystem.h"
#include "AuthException.h"
#include "BusinessRuleException.h"
#include "DataFormatException.h"
#include "FileException.h"
#include "InputClosedException.h"
#include "NotFoundException.h"
#include "Parsing.h"
#include "RentalException.h"
#include "Validation.h"
#include "ValidationException.h"
#include "Bulldozer.h"
#include "ConcreteMixer.h"
#include "Crane.h"
#include "Excavator.h"
#include "Loader.h"
#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <system_error>

namespace {

void warn(const std::string& file, std::size_t lineNo, const std::string& message) {
    std::cerr << "Предупреждение: " << file << ", строка " << lineNo << ": "
              << message << " (строка пропущена)\n";
}

// Читает все строки файла. Если файла нет (первый запуск) — возвращает пустой список.
std::vector<std::string> readLines(const std::string& path) {
    std::vector<std::string> lines;

    std::error_code ec;
    const bool exists = std::filesystem::exists(path, ec);
    if (ec) throw FileException("Не удалось проверить файл: " + path + " (" + ec.message() + ")");
    if (!exists) return lines;

    std::ifstream in(path);
    if (!in) throw FileException("Не удалось открыть файл для чтения: " + path);

    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back(); // файлы, правленные в Windows
        lines.push_back(line);
    }
    if (in.bad()) throw FileException("Ошибка чтения файла: " + path);
    return lines;
}

// Безопасная запись: сначала во временный файл, затем замена основного.
// При сбое старое содержимое файла не теряется.
void writeLines(const std::string& path, const std::vector<std::string>& lines) {
    const std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp, std::ios::trunc);
        if (!out) throw FileException("Не удалось открыть файл для записи: " + tmp);
        for (const auto& l : lines) out << l << '\n';
        out.flush();
        if (!out) throw FileException("Ошибка записи в файл: " + tmp);
    }
    std::error_code ec;
    std::filesystem::rename(tmp, path, ec);
    if (ec) {
        const std::string reason = ec.message();
        std::filesystem::remove(tmp, ec);
        throw FileException("Не удалось сохранить файл " + path + " (" + reason + ")");
    }
}

} // namespace

// ======================= Конструктор / файлы =======================
RentalSystem::RentalSystem(const std::string& equipmentPath,
                           const std::string& customersPath,
                           const std::string& ordersPath)
    : equipmentFile(equipmentPath), customersFile(customersPath), ordersFile(ordersPath) {}

void RentalSystem::loadEquipment() {
    equipment.clear();
    const auto lines = readLines(equipmentFile);
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) continue;
        try {
            auto item = Equipment::deserialize(lines[i]);
            if (findEquipment(item->getId())) throw DataFormatException("повторяющийся ID техники");
            equipment.push_back(std::move(item));
        } catch (const RentalException& e) {
            warn(equipmentFile, i + 1, e.what());
        }
    }
}

void RentalSystem::loadCustomers() {
    customers.clear();
    const auto lines = readLines(customersFile);
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) continue;
        try {
            Customer c = Customer::deserialize(lines[i]);
            if (findCustomerById(c.getId()) || findCustomerByLogin(c.getLogin()))
                throw DataFormatException("повторяющийся ID или логин клиента");
            customers.push_back(std::move(c));
        } catch (const RentalException& e) {
            warn(customersFile, i + 1, e.what());
        }
    }
    // Если клиентов нет — создаём администратора по умолчанию (admin / admin)
    if (customers.empty())
        customers.emplace_back(1, "admin", Customer::hashPassword("admin"),
                                "Администратор", "-", true);
}

void RentalSystem::loadOrders() {
    orders.clear();
    const auto lines = readLines(ordersFile);
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) continue;
        try {
            Order o = Order::deserialize(lines[i]);
            const bool duplicate = std::any_of(orders.begin(), orders.end(),
                [&o](const Order& other) { return other.id == o.id; });
            if (duplicate) throw DataFormatException("повторяющийся ID заказа");
            orders.push_back(o);
        } catch (const RentalException& e) {
            warn(ordersFile, i + 1, e.what());
        }
    }
}

void RentalSystem::saveEquipment() const {
    std::vector<std::string> lines;
    for (const auto& e : equipment) lines.push_back(e->serialize());
    writeLines(equipmentFile, lines);
}

void RentalSystem::saveCustomers() const {
    std::vector<std::string> lines;
    for (const auto& c : customers) lines.push_back(c.serialize());
    writeLines(customersFile, lines);
}

void RentalSystem::saveOrders() const {
    std::vector<std::string> lines;
    for (const auto& o : orders) lines.push_back(o.serialize());
    writeLines(ordersFile, lines);
}

void RentalSystem::loadAll() { loadEquipment(); loadCustomers(); loadOrders(); }
void RentalSystem::saveAll() const { saveEquipment(); saveCustomers(); saveOrders(); }

// ======================= Поиск и идентификаторы =======================
Equipment* RentalSystem::findEquipment(int id) {
    for (auto& e : equipment) if (e->getId() == id) return e.get();
    return nullptr;
}

Equipment& RentalSystem::requireEquipment(int id) {
    Equipment* e = findEquipment(id);
    if (!e) throw NotFoundException("Техника с ID " + std::to_string(id) + " не найдена");
    return *e;
}

Customer* RentalSystem::findCustomerByLogin(const std::string& loginName) {
    for (auto& c : customers) if (c.getLogin() == loginName) return &c;
    return nullptr;
}

Customer* RentalSystem::findCustomerById(int id) {
    for (auto& c : customers) if (c.getId() == id) return &c;
    return nullptr;
}

int RentalSystem::nextEquipmentId() const {
    int m = 0;
    for (const auto& e : equipment) m = std::max(m, e->getId());
    return m + 1;
}

int RentalSystem::nextCustomerId() const {
    int m = 0;
    for (const auto& c : customers) m = std::max(m, c.getId());
    return m + 1;
}

int RentalSystem::nextOrderId() const {
    int m = 0;
    for (const auto& o : orders) m = std::max(m, o.id);
    return m + 1;
}

// ======================= Ввод =======================
std::string RentalSystem::readLine(const std::string& prompt) {
    std::string s;
    std::cout << prompt;
    if (!std::getline(std::cin, s)) throw InputClosedException("Поток ввода закрыт");
    if (!s.empty() && s.back() == '\r') s.pop_back();
    // символ '|' — разделитель в файлах, в данных он недопустим
    std::replace(s.begin(), s.end(), '|', '/');
    return s;
}

int RentalSystem::readInt(const std::string& prompt, int minV, int maxV) {
    while (true) {
        const std::string s = readLine(prompt);
        try {
            const int v = parseInt(s, "ввод");
            if (v >= minV && v <= maxV) return v;
        } catch (const DataFormatException&) {
            // некорректный ввод — запрашиваем повторно
        }
        std::cout << "Введите целое число от " << minV << " до " << maxV << ".\n";
    }
}

double RentalSystem::readDouble(const std::string& prompt, double minV) {
    while (true) {
        const std::string s = readLine(prompt);
        try {
            const double v = parseDouble(s, "ввод");
            if (v >= minV) return v;
        } catch (const DataFormatException&) {
            // некорректный ввод — запрашиваем повторно
        }
        std::cout << "Введите число не меньше " << minV << ".\n";
    }
}

std::string RentalSystem::currentDate() {
    const std::time_t t = std::time(nullptr);
    std::tm tmLocal{};
#ifdef _WIN32
    if (localtime_s(&tmLocal, &t) != 0)   // MSVC: безопасная версия (аргументы: tm*, time_t*)
        throw RentalException("Не удалось получить текущую дату");
#else
    if (localtime_r(&t, &tmLocal) == nullptr)   // Linux/macOS
        throw RentalException("Не удалось получить текущую дату");
#endif
    char buf[16];
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tmLocal) == 0)
        throw RentalException("Не удалось отформатировать дату");
    return buf;
}

// ======================= Пользователи =======================
void RentalSystem::registerCustomer() {
    const std::string userLogin = readLine("Логин: ");
    if (userLogin.empty() || userLogin.find(' ') != std::string::npos)
        throw ValidationException("Логин не должен быть пустым или содержать пробелы");
    if (findCustomerByLogin(userLogin))
        throw ValidationException("Такой логин уже занят");

    const std::string pass = readLine("Пароль: ");
    Customer::validatePassword(pass);
    const std::string name = readLine("ФИО: ");
    const std::string phone = readLine("Телефон: ");

    // конструктор Customer проверит ФИО и телефон (ValidationException)
    customers.emplace_back(nextCustomerId(), userLogin, Customer::hashPassword(pass), name, phone, false);
    try {
        saveCustomers();
    } catch (...) {
        customers.pop_back(); // откат: в файл запись не попала
        throw;
    }
    std::cout << "Регистрация успешна.\n";
}

void RentalSystem::login() {
    const std::string userLogin = readLine("Логин: ");
    const std::string pass = readLine("Пароль: ");
    Customer* c = findCustomerByLogin(userLogin);
    if (!c || !c->checkPassword(pass))
        throw AuthException("Неверный логин или пароль");
    current = c;
    std::cout << "Добро пожаловать, " << c->getFullName() << "!\n";
}

void RentalSystem::showCustomers() const {
    std::cout << "\n--- Клиенты ---\n";
    for (const auto& c : customers)
        std::cout << c.getId() << ". " << c.getLogin() << " | " << c.getFullName()
                  << " | " << c.getPhone() << (c.isAdmin() ? " [admin]" : "") << "\n";
}

// ======================= Техника =======================
void RentalSystem::showEquipment(bool onlyAvailable) const {
    std::cout << "\n--- Каталог техники ---\n";
    bool any = false;
    for (const auto& e : equipment) {
        if (onlyAvailable && !e->isAvailable()) continue;
        e->print(std::cout);
        any = true;
    }
    if (!any) std::cout << "(пусто)\n";
}

void RentalSystem::addEquipment() {
    std::cout << "Тип: 1-Экскаватор 2-Автокран 3-Бульдозер 4-Бетономешалка 5-Погрузчик\n";
    const int type = readInt("Выбор: ", 1, 5);
    const std::string name = readLine("Название/модель: ");
    const double rate = readDouble("Тариф за сутки: ", 0.01);
    const int id = nextEquipmentId();

    std::unique_ptr<Equipment> item;
    switch (type) {
        case 1:
            item = std::make_unique<Excavator>(id, name, rate, readDouble("Объём ковша, м3: ", 0.01));
            break;
        case 2: {
            const double cap = readDouble("Грузоподъёмность, т: ", 0.01);
            const double boom = readDouble("Длина стрелы, м: ", 0.01);
            item = std::make_unique<Crane>(id, name, rate, cap, boom);
            break;
        }
        case 3: {
            const double blade = readDouble("Ширина отвала, м: ", 0.01);
            const int power = readInt("Мощность, л.с.: ", 1, 5000);
            item = std::make_unique<Bulldozer>(id, name, rate, blade, power);
            break;
        }
        case 4:
            item = std::make_unique<ConcreteMixer>(id, name, rate, readDouble("Объём барабана, м3: ", 0.01));
            break;
        default:
            item = std::make_unique<Loader>(id, name, rate, readDouble("Грузоподъёмность, т: ", 0.01));
            break;
    }

    equipment.push_back(std::move(item));
    try {
        saveEquipment();
    } catch (...) {
        equipment.pop_back(); // откат
        throw;
    }
    std::cout << "Техника добавлена (ID " << id << ").\n";
}

void RentalSystem::removeEquipment() {
    showEquipment(false);
    const int id = readInt("ID для удаления (0 - отмена): ", 0, 1000000);
    if (id == 0) return;

    const Equipment& e = requireEquipment(id);
    if (!e.isAvailable())
        throw BusinessRuleException("Нельзя удалить технику, находящуюся в аренде");

    const auto it = std::find_if(equipment.begin(), equipment.end(),
        [id](const std::unique_ptr<Equipment>& p) { return p->getId() == id; });
    const auto index = it - equipment.begin();
    std::unique_ptr<Equipment> removed = std::move(equipment[static_cast<std::size_t>(index)]);
    equipment.erase(it);
    try {
        saveEquipment();
    } catch (...) {
        equipment.insert(equipment.begin() + index, std::move(removed)); // откат
        throw;
    }
    std::cout << "Удалено.\n";
}

// ======================= Аренда =======================
void RentalSystem::rentEquipment() {
    showEquipment(true);
    const int id = readInt("ID техники (0 - отмена): ", 0, 1000000);
    if (id == 0) return;

    Equipment& e = requireEquipment(id);
    if (!e.isAvailable())
        throw BusinessRuleException("Техника уже находится в аренде");

    const int days = readInt("Количество суток (1-365): ", 1, 365);
    const double cost = e.calculateCost(days);
    std::cout << "Итоговая стоимость: " << std::fixed << std::setprecision(2) << cost << "\n";
    if (readLine("Подтвердить аренду? (y/n): ") != "y") {
        std::cout << "Аренда отменена.\n";
        return;
    }

    Order order;
    order.id = nextOrderId();
    order.customerId = current->getId();
    order.equipmentId = e.getId();
    order.days = days;
    order.totalCost = cost;
    order.startDate = currentDate();
    order.returned = false;

    orders.push_back(order);
    e.setAvailable(false);
    try {
        saveOrders();
        saveEquipment();
    } catch (...) {
        orders.pop_back();    // откат изменений в памяти
        e.setAvailable(true);
        throw;
    }
    std::cout << "Заказ оформлен.\n";
}

void RentalSystem::returnEquipment() {
    showMyOrders();
    const int id = readInt("ID заказа для возврата (0 - отмена): ", 0, 1000000);
    if (id == 0) return;

    const auto it = std::find_if(orders.begin(), orders.end(), [&](const Order& o) {
        return o.id == id && o.customerId == current->getId() && !o.returned;
    });
    if (it == orders.end())
        throw NotFoundException("Активный заказ с таким ID не найден");

    Equipment* e = findEquipment(it->equipmentId);
    if (!e)
        throw DataFormatException("Техника из заказа не найдена в каталоге — данные повреждены");

    it->returned = true;
    e->setAvailable(true);
    try {
        saveOrders();
        saveEquipment();
    } catch (...) {
        it->returned = false;   // откат изменений в памяти
        e->setAvailable(false);
        throw;
    }
    std::cout << "Техника возвращена.\n";
}

void RentalSystem::showMyOrders() const {
    std::cout << "\n--- Мои заказы ---\n";
    bool any = false;
    for (const auto& o : orders) {
        if (o.customerId != current->getId()) continue;
        std::cout << "Заказ #" << o.id << " | техника ID " << o.equipmentId
                  << " | " << o.days << " сут. | " << std::fixed << std::setprecision(2)
                  << o.totalCost << " | с " << o.startDate
                  << " | " << (o.returned ? "возвращена" : "активен") << "\n";
        any = true;
    }
    if (!any) std::cout << "(нет заказов)\n";
}

void RentalSystem::showAllOrders() const {
    std::cout << "\n--- Все заказы ---\n";
    double revenue = 0;
    for (const auto& o : orders) {
        std::cout << "Заказ #" << o.id << " | клиент ID " << o.customerId
                  << " | техника ID " << o.equipmentId << " | " << o.days << " сут. | "
                  << std::fixed << std::setprecision(2) << o.totalCost
                  << " | " << o.startDate << " | " << (o.returned ? "возвращена" : "активен") << "\n";
        revenue += o.totalCost;
    }
    std::cout << "Общая сумма заказов: " << revenue << "\n";
}

// ======================= Меню =======================
bool RentalSystem::guestMenu() {
    std::cout << "\n=== Аренда строительной техники ===\n"
                 "1. Войти\n2. Регистрация\n0. Выход\n";
    switch (readInt("Выбор: ", 0, 2)) {
        case 1: login(); break;
        case 2: registerCustomer(); break;
        default: return false;
    }
    return true;
}

void RentalSystem::customerMenu() {
    std::cout << "\n=== Меню клиента: " << current->getLogin() << " ===\n"
                 "1. Каталог техники\n2. Арендовать\n3. Мои заказы\n"
                 "4. Вернуть технику\n0. Выйти из аккаунта\n";
    switch (readInt("Выбор: ", 0, 4)) {
        case 1: showEquipment(false); break;
        case 2: rentEquipment(); break;
        case 3: showMyOrders(); break;
        case 4: returnEquipment(); break;
        default: current = nullptr; break;
    }
}

void RentalSystem::adminMenu() {
    std::cout << "\n=== Меню администратора ===\n"
                 "1. Каталог техники\n2. Добавить технику\n3. Удалить технику\n"
                 "4. Все заказы\n5. Список клиентов\n0. Выйти из аккаунта\n";
    switch (readInt("Выбор: ", 0, 5)) {
        case 1: showEquipment(false); break;
        case 2: addEquipment(); break;
        case 3: removeEquipment(); break;
        case 4: showAllOrders(); break;
        case 5: showCustomers(); break;
        default: current = nullptr; break;
    }
}

void RentalSystem::run() {
    // Если данные не удалось прочитать, продолжать нельзя: последующее сохранение затёрло бы файлы
    try {
        loadAll();
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка загрузки данных: " << e.what() << "\n";
        return;
    }

    bool running = true;
    while (running) {
        try {
            if (!current) running = guestMenu();
            else if (current->isAdmin()) adminMenu();
            else customerMenu();
        } catch (const InputClosedException&) {
            std::cout << "\nВвод завершён, выход из программы.\n";
            running = false;
        } catch (const RentalException& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Непредвиденная ошибка: " << e.what() << "\n";
        }
    }

    try {
        saveAll();
    } catch (const std::exception& e) {
        std::cerr << "Не удалось сохранить данные при выходе: " << e.what() << "\n";
    }
}
