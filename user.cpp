#include "database.h"
#include "utils.h"
#include "handlers.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>


using namespace std;
using namespace sql;

void PrintUserHeader() {
    cout << left
        << setw(6) << "ID"
        << setw(20) << "Логин"
        << setw(20) << "Пароль"
        << endl;
    cout << setfill('-') << setw(46) << "" << setfill(' ') << endl;
}

void Add_User() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string log, pas;

        cout << "\nВведите логин: ";
        log = read_utf8_line("");

        PreparedStatement* checkStmt = con->prepareStatement("SELECT login FROM users WHERE login = ?");
        checkStmt->setString(1, log);
        ResultSet* result = checkStmt->executeQuery();

        if (result->next()) {
            cout << "Пользователь уже существует. Enter...\n"; cin.get(); system("cls");
            delete result; delete checkStmt; return;
        }

        cout << "Введите пароль: ";
        pas = Get_Password();

        PreparedStatement* insertStmt = con->prepareStatement("INSERT INTO users (login, password, access) VALUES (?, ?, ?)");
        insertStmt->setString(1, log);
        insertStmt->setString(2, sha256(pas));
        insertStmt->setBoolean(3, true);

        int affected = insertStmt->executeUpdate();
        cout << (affected > 0 ? "Пользователь добавлен." : "Не удалось добавить.") << " Enter...\n";
        cin.get(); system("cls");

        delete insertStmt; delete result; delete checkStmt;
    }
    catch (SQLException& e) {
        cerr << "SQL ошибка: " << e.what() << endl;
    }
}

//Удаление пользователя
void Delete_User() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string s;

        // Ввод ID пользователя
        cout << "\nВведите ID пользователя для удаления: ";
        cin >> s;
        clearInputBuffer();

        // Проверка корректности ввода
        if (prov(s) != 1) {
            cout << "Некорректный ввод ID. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }
        int user_id = stoi(s);

        // Проверка существования пользователя
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT id, login FROM users WHERE id = ?"
        );
        checkStmt->setInt(1, user_id);
        ResultSet* result = checkStmt->executeQuery();

        if (!result->next()) {
            cout << "Пользователь с таким ID не найден. Введите Enter для продолжения...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        string full_name = result->getString("login");
        cout << "Пользователь найден: " << full_name << endl;

        // Подтверждение удаления
        cout << "Вы ТОЧНО хотите удалить этого пользователя?\n1 - Да\n0 - Нет\n";
        char choice;
        cin >> choice;
        clearInputBuffer();

        if (choice != '1') {
            delete result;
            delete checkStmt;
            system("cls");
            return;
        }

        // Удаление пользователя
        PreparedStatement* deleteStmt = con->prepareStatement(
            "DELETE FROM users WHERE id = ?"
        );
        deleteStmt->setInt(1, user_id);

        int affected_rows = deleteStmt->executeUpdate();

        if (affected_rows > 0) {
            cout << "Пользователь успешно удален. Введите Enter для продолжения...\n";
        }
        else {
            cout << "Не удалось удалить пользователя. Введите Enter для продолжения...\n";
        }

        // Освобождение ресурсов
        delete deleteStmt;
        delete result;
        delete checkStmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при удалении пользователя: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Вывод всех пользователей
void Read_User() {
    try {
        Database db;
        Connection* con = db.getConnection();

        // Создание и выполнение запроса
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery(
            "SELECT id, login, password FROM users WHERE access = TRUE"
        );

        // Вывод заголовка
        PrintUserHeader();
        bool hasData = false;

        // Вывод данных
        while (res->next()) {
            hasData = true;
            cout << left
                << setw(ID_WIDTH) << res->getInt("id")
                << setw(LOGIN_WIDTH) << res->getString("login")
                << setw(PASS_WIDTH) << res->getString("password")
                << endl;
        }

        if (!hasData) {
            cout << "Список пользователей пуст\n";
        }

        // Освобождение ресурсов
        delete res;
        delete stmt;

        cout << "Нажмите Enter для продолжения\n";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка чтения пользователей: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Регистрация пользователя(альтернативная)
void User_Registration() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string log, pas;

        // Ввод логина
        cout << "\nВведите логин: ";
        log = read_utf8_line("");

        // Проверка уникальности логина
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT login FROM users WHERE login = ?"
        );
        checkStmt->setString(1, log);
        ResultSet* result = checkStmt->executeQuery();

        if (result->next()) {
            cout << "Пользователь с таким логином уже существует. Введите Enter для продолжения...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        // Ввод пароля
        cout << "Введите пароль: ";
        pas = Get_Password(); // Функция с скрытым вводом пароля

        // Вставка нового пользователя
        PreparedStatement* insertStmt = con->prepareStatement(
            "INSERT INTO users (login, password, access) "
            "VALUES (?, ?, ?)"
        );

        insertStmt->setString(1, log);
        insertStmt->setString(2, sha256(pas));
        insertStmt->setBoolean(3, false); // access = 0

        int affected_rows = insertStmt->executeUpdate();

        if (affected_rows > 0) {
            cout << "Регистрация успешна. Доступ будет открыт после подтверждения администратора. Введите Enter для продолжения...\n";
        }
        else {
            cout << "Ошибка регистрации. Введите Enter для продолжения...\n";
        }

        // Освобождение ресурсов
        delete insertStmt;
        delete result;
        delete checkStmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при регистрации: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Подтверждение пользователя
void Verify_User() {
    try {
        Database db;
        Connection* con = db.getConnection();
        bool hasUnverified = false;

        do {
            // Получаем всех неподтвержденных пользователей
            PreparedStatement* selectStmt = con->prepareStatement(
                "SELECT id, login, password FROM users WHERE access = FALSE"
            );
            ResultSet* res = selectStmt->executeQuery();
            hasUnverified = false;

            while (res->next()) {
                hasUnverified = true;
                int user_id = res->getInt("id");
                string login = res->getString("login");
                string password = res->getString("password");

                // Вывод информации о пользователе
                cout << "\nID: " << user_id
                    << "\nЛогин: " << login
                    << "\nПароль: " << password
                    << "\n\n1 - Подтвердить аккаунт\n0 - Отклонить/Удалить\nВыбор: ";

                char choice;
                cin >> choice;
                clearInputBuffer();

                switch (choice) {
                case '1': {
                    // Подтверждение аккаунта
                    PreparedStatement* updateStmt = con->prepareStatement(
                        "UPDATE users SET access = TRUE WHERE id = ?"
                    );
                    updateStmt->setInt(1, user_id);

                    if (updateStmt->executeUpdate() > 0) {
                        cout << "Аккаунт подтвержден.\n";
                        cin.get();
                    }
                    delete updateStmt;
                    break;
                }
                case '0': {
                        PreparedStatement* deleteStmt = con->prepareStatement(
                            "DELETE FROM users WHERE id = ?"
                        );
                        deleteStmt->setInt(1, user_id);

                        if (deleteStmt->executeUpdate() > 0) {
                            cout << "Аккаунт удален.\n";
                                cin.get();
                        }
                        delete deleteStmt;
                    break;
                }
                default: {
                    cout << "Некорректный ввод. Пропускаем пользователя.\n";
                    break;
                }
                }
                system("cls");
            }

            delete res;
            delete selectStmt;

        } while (hasUnverified); // Повторяем, пока есть неподтвержденные

        cout << "Неподтвержденных аккаунтов нет.\nНажмите Enter для продолжения...";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cout << "Ошибка SQL: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cin.get();
        system("cls");
    }
}

//Функция для создания первого админа, работает при первом запуске программы
void FirstRunSetup() {
    try {
        Database db;
        Connection* con = db.getConnection();

        // Проверяем наличие администраторов
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery("SELECT COUNT(*) FROM admins");

        res->next();
        if (res->getInt(1) == 0) {
            cout << "=== Первый запуск системы ===" << endl;
            cout << "Создание администратора:" << endl;

            string login, password;

            cout << "Введите логин: ";
            cin >> login;
            clearInputBuffer();

            cout << "Введите пароль: ";
            password = Get_Password(); // Скрытый ввод

            // Хеширование пароля
            string hashed_pw = sha256(password);

            // Добавление в базу
            PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO admins (login, password) VALUES (?, ?)"
            );
            pstmt->setString(1, login);
            pstmt->setString(2, hashed_pw);
            pstmt->executeUpdate();

            cout << "\nАдминистратор создан! Теперь вы можете войти в систему.\n";
            // cout << "Добавлен администратор: " << login << " / " << hashed_pw << endl;//dobavil
            delete pstmt;
        }

        delete res;
        delete stmt;
    }
    catch (SQLException& e) {
        cerr << "Ошибка инициализации: " << e.what() << endl;
        exit(1);
    }
}

//Функция входа в систему
void Enter() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string l, p;
        // Ввод данных
        cout << "Введите логин: ";
        l = read_utf8_line("");

        cout << "Введите пароль: ";
        p = Get_Password();
        string hashed_input = sha256(p);

        // 1. Проверка администратора
        PreparedStatement* adminStmt = con->prepareStatement(
            "SELECT id FROM admins WHERE login = ? AND password = ?"
        );

        adminStmt->setString(1, l);

        adminStmt->setString(2, hashed_input);

        ResultSet* adminRes = adminStmt->executeQuery();

        if (adminRes->next()) {
            int admin_id = adminRes->getInt("id");
            delete adminRes;
            delete adminStmt;
            system("cls");
            Menu(admin_id, true);
            return;
        }
        delete adminRes;
        delete adminStmt;

        // 2. Проверка обычного пользователя
        PreparedStatement* userStmt = con->prepareStatement(
            "SELECT id, access FROM users WHERE login = ? AND password = ?"
        );
        userStmt->setString(1, l);
        userStmt->setString(2, sha256(p));

        ResultSet* userRes = userStmt->executeQuery();

        if (userRes->next()) {
            int user_id = userRes->getInt("id");
            bool access = userRes->getBoolean("access");

            if (access) {
                delete userRes;
                delete userStmt;
                system("cls");
                Menu(user_id, false);
                return;
            }
            else {
                cout << "Аккаунт не подтвержден администратором!\n";
            }
        }
        else {
            cout << "Неверный логин или пароль!\n";
        }

        delete userRes;
        delete userStmt;

        cout << "Введите Enter для продолжения...";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при входе: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Функция меню вошедшего пользователя, есть режим пользователя и администратора
void Menu(int user_id, bool is_admin) {
    char choice = '1';
    try {
        Database db;
        Connection* con = db.getConnection();

        // Выбираем правильную таблицу в зависимости от типа пользователя
        string query = is_admin
            ? "SELECT login FROM admins WHERE id = ?"
            : "SELECT login, access FROM users WHERE id = ?";

        PreparedStatement* userStmt = con->prepareStatement(query);
        userStmt->setInt(1, user_id);
        ResultSet* userRes = userStmt->executeQuery();

        if (!userRes->next()) {
            cout << "Ошибка: пользователь не найден!\n";
            delete userRes;
            delete userStmt;
            return;
        }

        // Получаем данные пользователя
        string current_login = userRes->getString("login");
        bool current_access = is_admin ? true : userRes->getBoolean("access");

        delete userRes;
        delete userStmt;

        while (choice != '0') {
            system("cls");

            // Вывод приветствия
            if (is_admin) {
                cout << "-------------------------------------------\n"
                    << "   Добро пожаловать, " << current_login << "\n"
                    << "      МЕНЮ АДМИНИСТРАТОРА\n"
                    << "-------------------------------------------\n";
            }
            else {
                if (current_access) {
                    cout << "-------------------------------------------\n"
                        << "    Добро пожаловать, " << current_login << "\n"
                        << "        МЕНЮ ПОЛЬЗОВАТЕЛЯ\n"
                        << "-------------------------------------------\n";
                }
                else {
                    cout << "\nВаш аккаунт ещё не подтверждён администратором\n"
                        << "Нажмите Enter для возврата в главное меню...";
                    cin.get();
                    return;
                }
            }

            // Основное меню
            cout << "\nОсновные операции:\n"
                << "1 - Добавить сотрудника\n"
                << "2 - Изменить данные сотрудника\n"
                << "3 - Удалить сотрудника\n"
                << "4 - Фильтр сотрудников\n"
                << "5 - Поиск сотрудника\n"
                << "6 - Сортировка сотрудников\n"
                << "7 - Показать всех сотрудников\n";

            // Дополнительные опции для администратора
            if (is_admin) {
                cout << "\nУправление пользователями:\n"
                    << "a - Добавить пользователя\n"
                    << "b - Удалить пользователя\n"
                    << "c - Просмотр всех пользователей\n"
                    << "d - Подтверждение регистраций\n";
            }

            cout << "\n0 - Выход из аккаунта\n"
                << "-------------------------------------------\n"
                << "Выберите операцию: ";

            cin >> choice;
            clearInputBuffer();
            system("cls");

            switch (choice) {
                // Общие операции
            case '1': Add_Employee(); break;
            case '2': Change_Employee(); break;
            case '3': Delete_Employee(); break;
            case '4': Filter_Employee(); break;
            case '5': Search_Employee(); break;
            case '6': Sort_Employee(); break;
            case '7': Read_Employee(); break;

                // Опции администратора
            case 'a': if (is_admin) Add_User(); break;
            case 'b': if (is_admin) Delete_User(); break;
            case 'c': if (is_admin) Read_User(); break;
            case 'd': if (is_admin) Verify_User(); break;

            case '0': break;

            default:
                cout << "Некорректный выбор! Пожалуйста, попробуйте снова.\n";
                cin.get();
                break;
            }
        }
    }
    catch (SQLException& e) {
        cerr << "Ошибка базы данных: " << e.what()
            << " (Код ошибки: " << e.getErrorCode()
            << ", SQL State: " << e.getSQLState() << ")\n";
        cin.get();
    }
    catch (const exception& e) {
        cerr << "Системная ошибка: " << e.what() << endl;
        cin.get();
    }
}