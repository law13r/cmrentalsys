#include "Customer.h"
#include "DataFormatException.h"
#include "Equipment.h" // splitString
#include "Parsing.h"
#include "Validation.h"
#include <functional>
#include <sstream>

Customer::Customer(int customerId, const std::string& userLogin, const std::string& hash,
                   const std::string& userName, const std::string& phoneNumber, bool adminFlag)
    : id(customerId), login(userLogin), passwordHash(hash),
      fullName(userName), phone(phoneNumber), admin(adminFlag) {
    if (id <= 0) throw ValidationException("ID клиента должен быть положительным");
    requireNotEmpty(login, "логин");
    requireNotEmpty(passwordHash, "пароль");
    requireNotEmpty(fullName, "ФИО");
    requireNotEmpty(phone, "телефон");
}

void Customer::setPhone(const std::string& newPhone) {
    requireNotEmpty(newPhone, "телефон");
    phone = newPhone;
}

void Customer::setFullName(const std::string& newName) {
    requireNotEmpty(newName, "ФИО");
    fullName = newName;
}

void Customer::validatePassword(const std::string& password) {
    if (password.size() < 4)
        throw ValidationException("Пароль должен быть не короче 4 символов");
}

std::string Customer::hashPassword(const std::string& password) {
    return std::to_string(std::hash<std::string>{}(password));
}

void Customer::changePassword(const std::string& newPassword) {
    validatePassword(newPassword);
    passwordHash = hashPassword(newPassword);
}

bool Customer::checkPassword(const std::string& password) const {
    return passwordHash == hashPassword(password);
}

std::string Customer::serialize() const {
    std::ostringstream os;
    os << id << '|' << login << '|' << passwordHash << '|'
       << fullName << '|' << phone << '|' << (admin ? 1 : 0);
    return os.str();
}

Customer Customer::deserialize(const std::string& line) {
    const auto f = splitString(line, '|');
    if (f.size() < 6)
        throw DataFormatException("Недостаточно полей в записи о клиенте");
    return Customer(parseInt(f[0], "ID"), f[1], f[2], f[3], f[4], parseBool(f[5], "администратор"));
}
