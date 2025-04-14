#include "database.h"
#include "utils.h"
#include "handlers.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <string>
#include <limits>
#include <cstdlib>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>


using namespace std;
using namespace sql;

//Вывод "шапки" вывода рабочих
void PrintEmployeeHeader() {
    cout << left
        << setw(6) << "ID"
        << setw(25) << "ФИО"
        << setw(10) << "Возраст"
        << setw(20) << "Должность"
        << setw(12) << "Зарплата"
        << endl;
    cout << setfill('-') << setw(73) << "" << setfill(' ') << endl;
}

//Форматированный вывод рабочего
void PrintEmployeeRow(const Employee& emp) {
    cout << left
        << setw(6) << emp.id
        << setw(25) << emp.full_name
        << setw(10) << emp.age
        << setw(20) << emp.Position
        << fixed << setprecision(2)
        << setw(12) << emp.salary
        << endl;
}

//Удаление рабочего
void Add_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string id_s, name_s, age_s, pos_s, sal_s;

        cout << "\nВведите ФИО: ";
        cin >> name_s;
        clearInputBuffer();

        cout << "Введите индекс: ";
        cin >> id_s;
        clearInputBuffer();
        if (prov(id_s) != 1) {
            cout << "Некорректный ввод ID. Enter...\n"; cin.get(); system("cls"); return;
        }

        cout << "Введите возраст: ";
        cin >> age_s;
        clearInputBuffer();
        if (prov(age_s) != 1) {
            cout << "Некорректный возраст. Enter...\n"; cin.get(); system("cls"); return;
        }

        cout << "Введите должность: ";
        cin >> pos_s;
        clearInputBuffer();

        cout << "Введите зарплату: ";
        cin >> sal_s;
        clearInputBuffer();
        if (prov(sal_s) < 1) {
            cout << "Некорректная зарплата. Enter...\n"; cin.get(); system("cls"); return;
        }

        PreparedStatement* checkStmt = con->prepareStatement("SELECT id FROM employees WHERE id = ? OR full_name = ?");
        checkStmt->setInt(1, stoi(id_s));
        checkStmt->setString(2, name_s);
        ResultSet* result = checkStmt->executeQuery();

        if (result->next()) {
            cout << "Сотрудник уже существует. Enter...\n"; cin.get(); system("cls");
            delete result; delete checkStmt; return;
        }

        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO employees (id, full_name, age, position, salary) VALUES (?, ?, ?, ?, ?)");
        pstmt->setInt(1, stoi(id_s));
        pstmt->setString(2, name_s);
        pstmt->setInt(3, stoi(age_s));
        pstmt->setString(4, pos_s);
        pstmt->setDouble(5, stold(sal_s));
        pstmt->execute();

        cout << "Сотрудник добавлен. Enter...\n";
        cin.get(); system("cls");

        delete pstmt; delete result; delete checkStmt;
    }
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
    }
}

//Изменеие рабочего
void Change_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string s;

        // Ввод ID сотрудника
        cout << "\nВведите ID сотрудника для изменения: ";
        cin >> s;
        clearInputBuffer();

        // Проверка корректности ввода ID
        if (prov(s) != 1) {
            cout << "Некорректный ввод ID. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }
        int employee_id = stoi(s);

        // Проверка существования сотрудника
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT id FROM employees WHERE id = ?"
        );
        checkStmt->setInt(1, employee_id);
        ResultSet* result = checkStmt->executeQuery();

        if (!result->next()) {
            cout << "Сотрудник с таким ID не найден. Введите Enter для продолжения...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        // Ввод новых данных
        string name_s, age_s, pos_s, sal_s;

        cout << "Введите новое ФИО: ";
        getline(cin, name_s);

        cout << "Введите новый возраст: ";
        getline(cin, age_s);
        if (prov(age_s) != 1) {
            cout << "Некорректный возраст. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }

        cout << "Введите новую должность: ";
        getline(cin, pos_s);

        cout << "Введите новую зарплату: ";
        getline(cin, sal_s);
        if (prov(sal_s) < 1) {
            cout << "Некорректная зарплата. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }

        // Обновление данных в базе
        PreparedStatement* updateStmt = con->prepareStatement(
            "UPDATE employees SET "
            "full_name = ?, "
            "age = ?, "
            "position = ?, "
            "salary = ? "
            "WHERE id = ?"
        );

        updateStmt->setString(1, name_s);
        updateStmt->setInt(2, stoi(age_s));
        updateStmt->setString(3, pos_s);
        updateStmt->setDouble(4, stold(sal_s));
        updateStmt->setInt(5, employee_id);

        int affected_rows = updateStmt->executeUpdate();

        if (affected_rows > 0) {
            cout << "Данные сотрудника успешно обновлены. Введите Enter для продолжения...\n";
        }
        else {
            cout << "Не удалось обновить данные сотрудника. Введите Enter для продолжения...\n";
        }

        // Очистка ресурсов
        delete updateStmt;
        delete result;
        delete checkStmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при изменении сотрудника: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Удаление рабочего
void Delete_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string s;

        // Ввод ID сотрудника
        cout << "\nВведите ID сотрудника для удаления: ";
        cin >> s;
        clearInputBuffer();

        // Проверка корректности ввода
        if (prov(s) != 1) {
            cout << "Некорректный ввод ID. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }
        int employee_id = stoi(s);

        // Проверка существования сотрудника
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT id FROM employees WHERE id = ?"
        );
        checkStmt->setInt(1, employee_id);
        ResultSet* result = checkStmt->executeQuery();

        if (!result->next()) {
            cout << "Сотрудник с таким ID не найден. Введите Enter для продолжения...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        string full_name = result->getString("full_name");
        cout << "Сотрудник найден: " << full_name << endl;

        // Подтверждение удаления
        cout << "Вы ТОЧНО хотите удалить этого сотрудника?\n1 - Да\n0 - Нет\n";
        char choice;
        cin >> choice;
        clearInputBuffer();

        if (choice != '1') {
            system("cls");
            delete result;
            delete checkStmt;
            return;
        }

        // Удаление из базы данных
        PreparedStatement* deleteStmt = con->prepareStatement(
            "DELETE FROM employees WHERE id = ?"
        );
        deleteStmt->setInt(1, employee_id);

        int affected_rows = deleteStmt->executeUpdate();

        if (affected_rows > 0) {
            cout << "Сотрудник успешно удален. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");

        }
        else {
            cout << "Не удалось удалить сотрудника. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
        }

        // Освобождение ресурсов
        delete deleteStmt;
        delete result;
        delete checkStmt;



    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при удалении сотрудника: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Фильтр рабочих(по трем полям + по трем операторам, итого - 9 вариаций)
void Filter_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string a = "-1", b = "-1";
        char t, s;

        // Выбор поля для фильтрации
        cout << "Фильтр по:\n";
        cout << "1 - ID\n";
        cout << "2 - Возрасту\n";
        cout << "3 - Зарплате\n";
        cin >> t;
        clearInputBuffer();

        if (t < '1' || t > '3') {
            cout << "Некорректный ввод. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }

        // Выбор оператора
        cout << "> - больше\n< - меньше\n# - в пределах\n";
        cin >> s;
        clearInputBuffer();

        // Определение имени поля для SQL-запроса
        string field_name;
        switch (t) {
        case '1': field_name = "id"; break;
        case '2': field_name = "age"; break;
        case '3': field_name = "salary"; break;
        }

        // Формирование условия запроса
        string where_clause;
        vector<double> params;

        switch (s) {
        case '>':
            cout << "Введите минимальное значение: ";
            cin >> a;
            clearInputBuffer();
            if (prov(a) < 1) {
                cout << "Некорректный ввод. Введите Enter для продолжения...\n";
                cin.get();
                system("cls");
                return;
            }
            where_clause = field_name + " > ?";
            params.push_back(stold(a));
            break;

        case '<':
            cout << "Введите максимальное значение: ";
            cin >> b;
            clearInputBuffer();
            if (prov(b) < 1) {
                cout << "Некорректный ввод. Введите Enter для продолжения...\n";
                cin.get();
                system("cls");
                return;
            }
            where_clause = field_name + " < ?";
            params.push_back(stold(b));
            break;

        case '#':
            cout << "Введите минимальное значение: ";
            cin >> a;
            clearInputBuffer();
            cout << "Введите максимальное значение: ";
            cin >> b;
            clearInputBuffer();
            if (prov(a) < 1 || prov(b) < 1) {
                cout << "Некорректный ввод. Введите Enter для продолжения...\n";
                cin.get();
                system("cls");
                return;
            }
            where_clause = field_name + " BETWEEN ? AND ?";
            params.push_back(stold(a));
            params.push_back(stold(b));
            break;

        default:
            cout << "Некорректный ввод. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }

        // Формирование SQL-запроса
        string sql = "SELECT * FROM employees WHERE " + where_clause;
        PreparedStatement* pstmt = con->prepareStatement(sql);

        // Подстановка параметров
        for (size_t i = 0; i < params.size(); ++i) {
            pstmt->setDouble(i + 1, params[i]);
        }

        // Выполнение запроса
        ResultSet* res = pstmt->executeQuery();

        // Вывод результатов
        PrintEmployeeHeader();
        bool found = false;

        while (res->next()) {
            found = true;
            Employee emp{
                res->getInt("id"),
                res->getString("full_name"),
                res->getInt("age"),
                res->getString("position"),
                res->getDouble("salary")
            };
            PrintEmployeeRow(emp);
        }

        if (!found) {
            cout << "Сотрудники не найдены. Введите Enter для продолжения...\n";
        }

        // Очистка ресурсов
        delete res;
        delete pstmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при фильтрации: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Поиск рабочего
void Search_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        char t;
        string s;

        // Выбор критерия поиска
        cout << "Поиск по:\n"
            << "1 - ID\n"
            << "2 - ФИО\n"
            << "3 - Возрасту\n"
            << "4 - Должности\n"
            << "5 - Зарплате\n";
        cin >> t;
        clearInputBuffer();

        if (t < '1' || t > '5') {
            cout << "Некорректный ввод. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }

        // Ввод значения для поиска
        cout << "\nВведите значение для поиска:\n";
        cin >> s;
        clearInputBuffer();

        // Валидация числовых полей
        if (t == '1' || t == '3' || t == '5') {
            if (prov(s) != (t == '5' ? 2 : 1)) {
                cout << "Некорректный ввод. Введите Enter для продолжения...\n";
                cin.get();
                system("cls");
                return;
            }
        }

        // Формирование SQL-запроса
        string field_name;
        string sql;
        unique_ptr<PreparedStatement> pstmt;

        switch (t) {
        case '1':
            sql = "SELECT * FROM employees WHERE id = ?";
            pstmt.reset(con->prepareStatement(sql));
            pstmt->setInt(1, stoi(s));
            break;

        case '2':
            sql = "SELECT * FROM employees WHERE full_name = ?";
            pstmt.reset(con->prepareStatement(sql));
            pstmt->setString(1, s);
            break;

        case '3':
            sql = "SELECT * FROM employees WHERE age = ?";
            pstmt.reset(con->prepareStatement(sql));
            pstmt->setInt(1, stoi(s));
            break;

        case '4':
            sql = "SELECT * FROM employees WHERE position = ?";
            pstmt.reset(con->prepareStatement(sql));
            pstmt->setString(1, s);
            break;

        case '5':
            sql = "SELECT * FROM employees WHERE salary = ?";
            pstmt.reset(con->prepareStatement(sql));
            pstmt->setDouble(1, stold(s));
            break;
        }

        // Выполнение запроса
        unique_ptr<ResultSet> res(pstmt->executeQuery());

        // Вывод результатов
        PrintEmployeeHeader();
        bool found = false;

        while (res->next()) {
            found = true;
            Employee emp{
                res->getInt("id"),
                res->getString("full_name"),
                res->getInt("age"),
                res->getString("position"),
                res->getDouble("salary")
            };
            PrintEmployeeRow(emp);
        }

        if (!found) {
            cout << "Сотрудник не найден. Введите Enter для продолжения...\n";
        }

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при поиске: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Сортировка рабочих(по пяти полям + по возрастанию и убыванию, итого - 10 вариаций)
void Sort_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        char x, t;

        // Выбор направления сортировки
        cout << "Сортировка по возрастанию(>) или по убыванию(<)?\n";
        cin >> x;
        clearInputBuffer();

        if (x != '>' && x != '<') {
            cout << "Некорректный ввод. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }

        // Выбор поля для сортировки
        cout << "Сортировка по:\n";
        cout << "1 - ID\n";
        cout << "2 - ФИО\n";
        cout << "3 - Возрасту\n";
        cout << "4 - Зарплате\n";
        cout << "5 - Должности\n";
        cin >> t;
        clearInputBuffer();

        // Определение параметров сортировки
        string field_name;
        string order_direction = (x == '>') ? "ASC" : "DESC";

        switch (t) {
        case '1': field_name = "id"; break;
        case '2': field_name = "full_name"; break;
        case '3': field_name = "age"; break;
        case '4': field_name = "salary"; break;
        case '5': field_name = "position"; break;
        default:
            cout << "Некорректный ввод. Введите Enter для продолжения...\n";
            cin.get();
            system("cls");
            return;
        }

        // Формирование SQL-запроса
        string sql = "SELECT * FROM employees ORDER BY " + field_name + " " + order_direction;
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery(sql);

        // Вывод отсортированных данных
        PrintEmployeeHeader();
        while (res->next()) {
            Employee emp{
                res->getInt("id"),
                res->getString("full_name"),
                res->getInt("age"),
                res->getString("position"),
                res->getDouble("salary")
            };
            PrintEmployeeRow(emp);
        }

        // Очистка ресурсов
        delete res;
        delete stmt;

        cout << "Сотрудники отсортированы. Введите Enter для продолжения...\n";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка SQL при сортировке: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//Вывод всех рабочих 
void Read_Employee() {
    try {
        system("cls");
        Database db;
        Connection* con = db.getConnection();

        // Создание и выполнение запроса
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery("SELECT * FROM employees");

        PrintEmployeeHeader();
        bool hasData = false;

        // Вывод результатов
        while (res->next()) {
            hasData = true;
            Employee emp{
                res->getInt("id"),
                res->getString("full_name"),
                res->getInt("age"),
                res->getString("position"),
                res->getDouble("salary")
            };
            PrintEmployeeRow(emp);
        }

        if (!hasData) {
            cout << "Список сотрудников пуст\n";
        }

        // Освобождение ресурсов
        delete res;
        delete stmt;

        cout << "\nНажмите Enter для продолжения...";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "Ошибка чтения данных: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}