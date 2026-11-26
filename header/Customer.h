#pragma once
#include <string>

class Customer {
    int id;
    std::string login;
    std::string passwordHash;
    std::string fullName;
    std::string phone;
    bool admin;

public:
    // Бросает ValidationException при некорректных параметрах
    Customer(int customerId, const std::string& userLogin, const std::string& hash,
             const std::string& userName, const std::string& phoneNumber, bool adminFlag = false);

    int getId() const { return id; }
    const std::string& getLogin() const { return login; }
    const std::string& getFullName() const { return fullName; }
    const std::string& getPhone() const { return phone; }
    bool isAdmin() const { return admin; }

    void setPhone(const std::string& newPhone);
    void setFullName(const std::string& newName);
    void changePassword(const std::string& newPassword);   // ValidationException, если пароль слабый
    bool checkPassword(const std::string& password) const;

    // Формат строки: id|login|hash|ФИО|телефон|admin
    std::string serialize() const;
    // Бросает DataFormatException (или ValidationException), если строка повреждена
    static Customer deserialize(const std::string& line);

    // Проверка требований к паролю (ValidationException)
    static void validatePassword(const std::string& password);

    // Хеширование пароля (учебный вариант на std::hash)
    static std::string hashPassword(const std::string& password);
};
