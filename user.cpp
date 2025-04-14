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
        << setw(20) << "�����"
        << setw(20) << "������"
        << endl;
    cout << setfill('-') << setw(46) << "" << setfill(' ') << endl;
}

void Add_User() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string log, pas;

        cout << "\n������� �����: ";
        cin >> log;
        clearInputBuffer();

        PreparedStatement* checkStmt = con->prepareStatement("SELECT login FROM users WHERE login = ?");
        checkStmt->setString(1, log);
        ResultSet* result = checkStmt->executeQuery();

        if (result->next()) {
            cout << "������������ ��� ����������. Enter...\n"; cin.get(); system("cls");
            delete result; delete checkStmt; return;
        }

        cout << "������� ������: ";
        pas = Get_Password();

        PreparedStatement* insertStmt = con->prepareStatement("INSERT INTO users (login, password, access) VALUES (?, ?, ?)");
        insertStmt->setString(1, log);
        insertStmt->setString(2, sha256(pas));
        insertStmt->setBoolean(3, true);

        int affected = insertStmt->executeUpdate();
        cout << (affected > 0 ? "������������ ��������." : "�� ������� ��������.") << " Enter...\n";
        cin.get(); system("cls");

        delete insertStmt; delete result; delete checkStmt;
    }
    catch (SQLException& e) {
        cerr << "SQL ������: " << e.what() << endl;
    }
}

//�������� ������������
void Delete_User() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string s;

        // ���� ID ������������
        cout << "\n������� ID ������������ ��� ��������: ";
        cin >> s;
        clearInputBuffer();

        // �������� ������������ �����
        if (prov(s) != 1) {
            cout << "������������ ���� ID. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }
        int user_id = stoi(s);

        // �������� ������������� ������������
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT id FROM users WHERE id = ?"
        );
        checkStmt->setInt(1, user_id);
        ResultSet* result = checkStmt->executeQuery();

        if (!result->next()) {
            cout << "������������ � ����� ID �� ������. ������� Enter ��� �����������...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        string full_name = result->getString("full_name");
        cout << "������������ ������: " << full_name << endl;

        // ������������� ��������
        cout << "�� ����� ������ ������� ����� ������������?\n1 - ��\n0 - ���\n";
        char choice;
        cin >> choice;
        clearInputBuffer();

        if (choice != '1') {
            delete result;
            delete checkStmt;
            system("cls");
            return;
        }

        // �������� ������������
        PreparedStatement* deleteStmt = con->prepareStatement(
            "DELETE FROM users WHERE id = ?"
        );
        deleteStmt->setInt(1, user_id);

        int affected_rows = deleteStmt->executeUpdate();

        if (affected_rows > 0) {
            cout << "������������ ������� ������. ������� Enter ��� �����������...\n";
        }
        else {
            cout << "�� ������� ������� ������������. ������� Enter ��� �����������...\n";
        }

        // ������������ ��������
        delete deleteStmt;
        delete result;
        delete checkStmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� �������� ������������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//����� ���� �������������
void Read_User() {
    try {
        Database db;
        Connection* con = db.getConnection();

        // �������� � ���������� �������
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery(
            "SELECT id, login, password FROM users WHERE access = TRUE"
        );

        // ����� ���������
        PrintUserHeader();
        bool hasData = false;

        // ����� ������
        while (res->next()) {
            hasData = true;
            cout << left
                << setw(ID_WIDTH) << res->getInt("id")
                << setw(LOGIN_WIDTH) << res->getString("login")
                << setw(PASS_WIDTH) << res->getString("password")
                << endl;
        }

        if (!hasData) {
            cout << "������ ������������� ����\n";
        }

        // ������������ ��������
        delete res;
        delete stmt;

        cout << "������� Enter ��� �����������\n";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ ������ �������������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//����������� ������������(��������������)
void User_Registration() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string log, pas;

        // ���� ������
        cout << "\n������� �����: ";
        cin >> log;
        clearInputBuffer();

        // �������� ������������ ������
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT login FROM users WHERE login = ?"
        );
        checkStmt->setString(1, log);
        ResultSet* result = checkStmt->executeQuery();

        if (result->next()) {
            cout << "������������ � ����� ������� ��� ����������. ������� Enter ��� �����������...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        // ���� ������
        cout << "������� ������: ";
        pas = Get_Password(); // ������� � ������� ������ ������

        // ������� ������ ������������
        PreparedStatement* insertStmt = con->prepareStatement(
            "INSERT INTO users (login, password, access) "
            "VALUES (?, ?, ?)"
        );

        insertStmt->setString(1, log);
        insertStmt->setString(2, sha256(pas));
        insertStmt->setBoolean(3, false); // access = 0

        int affected_rows = insertStmt->executeUpdate();

        if (affected_rows > 0) {
            cout << "����������� �������. ������ ����� ������ ����� ������������� ��������������. ������� Enter ��� �����������...\n";
        }
        else {
            cout << "������ �����������. ������� Enter ��� �����������...\n";
        }

        // ������������ ��������
        delete insertStmt;
        delete result;
        delete checkStmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� �����������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//������������� ������������
void Verify_User() {
    try {
        Database db;
        Connection* con = db.getConnection();
        bool hasUnverified = false;

        do {
            // �������� ���� ���������������� �������������
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

                // ����� ���������� � ������������
                cout << "\nID: " << user_id
                    << "\n�����: " << login
                    << "\n������: " << password
                    << "\n\n1 - ����������� �������\n0 - ���������/�������\n�����: ";

                char choice;
                cin >> choice;
                clearInputBuffer();

                switch (choice) {
                case '1': {
                    // ������������� ��������
                    PreparedStatement* updateStmt = con->prepareStatement(
                        "UPDATE users SET access = TRUE WHERE id = ?"
                    );
                    updateStmt->setInt(1, user_id);

                    if (updateStmt->executeUpdate() > 0) {
                        cout << "������� �����������.\n";
                    }
                    delete updateStmt;
                    break;
                }
                case '0': {
                    // �������� ��������
                    cout << "������� �������? (1 - ��, 0 - ���): ";
                    char deleteChoice;
                    cin >> deleteChoice;
                    clearInputBuffer();

                    if (deleteChoice == '1') {
                        PreparedStatement* deleteStmt = con->prepareStatement(
                            "DELETE FROM users WHERE id = ?"
                        );
                        deleteStmt->setInt(1, user_id);

                        if (deleteStmt->executeUpdate() > 0) {
                            cout << "������� ������.\n";
                        }
                        delete deleteStmt;
                    }
                    break;
                }
                default: {
                    cout << "������������ ����. ���������� ������������.\n";
                    break;
                }
                }
                system("cls");
            }

            delete res;
            delete selectStmt;

        } while (hasUnverified); // ���������, ���� ���� ����������������

        cout << "���������������� ��������� ���.\n������� Enter ��� �����������...";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//������� ��� �������� ������� ������, �������� ��� ������ ������� ���������
void FirstRunSetup() {
    try {
        Database db;
        Connection* con = db.getConnection();

        // ��������� ������� ���������������
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery("SELECT COUNT(*) FROM admins");

        res->next();
        if (res->getInt(1) == 0) {
            cout << "=== ������ ������ ������� ===" << endl;
            cout << "�������� ��������������:" << endl;

            string login, password;

            cout << "������� �����: ";
            cin >> login;
            clearInputBuffer();

            cout << "������� ������: ";
            password = Get_Password(); // ������� ����

            // ����������� ������
            string hashed_pw = sha256(password);

            // ���������� � ����
            PreparedStatement* pstmt = con->prepareStatement(
                "INSERT INTO admins (login, password) VALUES (?, ?)"
            );
            pstmt->setString(1, login);
            pstmt->setString(2, hashed_pw);
            pstmt->executeUpdate();

            cout << "\n������������� ������! ������ �� ������ ����� � �������.\n";
            // cout << "�������� �������������: " << login << " / " << hashed_pw << endl;//dobavil
            delete pstmt;
        }

        delete res;
        delete stmt;
    }
    catch (SQLException& e) {
        cerr << "������ �������������: " << e.what() << endl;
        exit(1);
    }
}

//������� ����� � �������
void Enter() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string l, p;
        // ���� ������
        cout << "������� �����: ";
        cin >> l;
        clearInputBuffer();

        cout << "������� ������: ";
        p = Get_Password();
        string hashed_input = sha256(p);

        // 1. �������� ��������������
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

        // 2. �������� �������� ������������
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
                cout << "������� �� ����������� ���������������!\n";
            }
        }
        else {
            cout << "�������� ����� ��� ������!\n";
        }

        delete userRes;
        delete userStmt;

        cout << "������� Enter ��� �����������...";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� �����: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//������� ���� ��������� ������������, ���� ����� ������������ � ��������������
void Menu(int user_id, bool is_admin) {
    char choice = '1';
    try {
        Database db;
        Connection* con = db.getConnection();

        // �������� ���������� ������� � ����������� �� ���� ������������
        string query = is_admin
            ? "SELECT login FROM admins WHERE id = ?"
            : "SELECT login, access FROM users WHERE id = ?";

        PreparedStatement* userStmt = con->prepareStatement(query);
        userStmt->setInt(1, user_id);
        ResultSet* userRes = userStmt->executeQuery();

        if (!userRes->next()) {
            cout << "������: ������������ �� ������!\n";
            delete userRes;
            delete userStmt;
            return;
        }

        // �������� ������ ������������
        string current_login = userRes->getString("login");
        bool current_access = is_admin ? true : userRes->getBoolean("access");

        delete userRes;
        delete userStmt;

        while (choice != '0') {
            system("cls");

            // ����� �����������
            if (is_admin) {
                cout << "-------------------------------------------\n"
                    << "   ����� ����������, " << current_login << "\n"
                    << "      ���� ��������������\n"
                    << "-------------------------------------------\n";
            }
            else {
                if (current_access) {
                    cout << "-------------------------------------------\n"
                        << "    ����� ����������, " << current_login << "\n"
                        << "        ���� ������������\n"
                        << "-------------------------------------------\n";
                }
                else {
                    cout << "\n��� ������� ��� �� ���������� ���������������\n"
                        << "������� Enter ��� �������� � ������� ����...";
                    cin.get();
                    return;
                }
            }

            // �������� ����
            cout << "\n�������� ��������:\n"
                << "1 - �������� ����������\n"
                << "2 - �������� ������ ����������\n"
                << "3 - ������� ����������\n"
                << "4 - ������ �����������\n"
                << "5 - ����� ����������\n"
                << "6 - ���������� �����������\n"
                << "7 - �������� ���� �����������\n";

            // �������������� ����� ��� ��������������
            if (is_admin) {
                cout << "\n���������� ��������������:\n"
                    << "a - �������� ������������\n"
                    << "b - ������� ������������\n"
                    << "c - �������� ���� �������������\n"
                    << "d - ������������� �����������\n";
            }

            cout << "\n0 - ����� �� ��������\n"
                << "-------------------------------------------\n"
                << "�������� ��������: ";

            cin >> choice;
            clearInputBuffer();
            system("cls");

            switch (choice) {
                // ����� ��������
            case '1': Add_Employee(); break;
            case '2': Change_Employee(); break;
            case '3': Delete_Employee(); break;
            case '4': Filter_Employee(); break;
            case '5': Search_Employee(); break;
            case '6': Sort_Employee(); break;
            case '7': Read_Employee(); break;

                // ����� ��������������
            case 'a': if (is_admin) Add_User(); break;
            case 'b': if (is_admin) Delete_User(); break;
            case 'c': if (is_admin) Read_User(); break;
            case 'd': if (is_admin) Verify_User(); break;

            case '0': break;

            default:
                cout << "������������ �����! ����������, ���������� �����.\n";
                cin.get();
                break;
            }
        }
    }
    catch (SQLException& e) {
        cerr << "������ ���� ������: " << e.what()
            << " (��� ������: " << e.getErrorCode()
            << ", SQL State: " << e.getSQLState() << ")\n";
        cin.get();
    }
    catch (const exception& e) {
        cerr << "��������� ������: " << e.what() << endl;
        cin.get();
    }
}