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

//����� "�����" ������ �������
void PrintEmployeeHeader() {
    cout << left
        << setw(6) << "ID"
        << setw(25) << "���"
        << setw(10) << "�������"
        << setw(20) << "���������"
        << setw(12) << "��������"
        << endl;
    cout << setfill('-') << setw(73) << "" << setfill(' ') << endl;
}

//��������������� ����� ��������
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

//�������� ��������
void Add_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string id_s, name_s, age_s, pos_s, sal_s;

        cout << "\n������� ���: ";
        cin >> name_s;
        clearInputBuffer();

        cout << "������� ������: ";
        cin >> id_s;
        clearInputBuffer();
        if (prov(id_s) != 1) {
            cout << "������������ ���� ID. Enter...\n"; cin.get(); system("cls"); return;
        }

        cout << "������� �������: ";
        cin >> age_s;
        clearInputBuffer();
        if (prov(age_s) != 1) {
            cout << "������������ �������. Enter...\n"; cin.get(); system("cls"); return;
        }

        cout << "������� ���������: ";
        cin >> pos_s;
        clearInputBuffer();

        cout << "������� ��������: ";
        cin >> sal_s;
        clearInputBuffer();
        if (prov(sal_s) < 1) {
            cout << "������������ ��������. Enter...\n"; cin.get(); system("cls"); return;
        }

        PreparedStatement* checkStmt = con->prepareStatement("SELECT id FROM employees WHERE id = ? OR full_name = ?");
        checkStmt->setInt(1, stoi(id_s));
        checkStmt->setString(2, name_s);
        ResultSet* result = checkStmt->executeQuery();

        if (result->next()) {
            cout << "��������� ��� ����������. Enter...\n"; cin.get(); system("cls");
            delete result; delete checkStmt; return;
        }

        PreparedStatement* pstmt = con->prepareStatement("INSERT INTO employees (id, full_name, age, position, salary) VALUES (?, ?, ?, ?, ?)");
        pstmt->setInt(1, stoi(id_s));
        pstmt->setString(2, name_s);
        pstmt->setInt(3, stoi(age_s));
        pstmt->setString(4, pos_s);
        pstmt->setDouble(5, stold(sal_s));
        pstmt->execute();

        cout << "��������� ��������. Enter...\n";
        cin.get(); system("cls");

        delete pstmt; delete result; delete checkStmt;
    }
    catch (SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
    }
}

//�������� ��������
void Change_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string s;

        // ���� ID ����������
        cout << "\n������� ID ���������� ��� ���������: ";
        cin >> s;
        clearInputBuffer();

        // �������� ������������ ����� ID
        if (prov(s) != 1) {
            cout << "������������ ���� ID. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }
        int employee_id = stoi(s);

        // �������� ������������� ����������
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT id FROM employees WHERE id = ?"
        );
        checkStmt->setInt(1, employee_id);
        ResultSet* result = checkStmt->executeQuery();

        if (!result->next()) {
            cout << "��������� � ����� ID �� ������. ������� Enter ��� �����������...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        // ���� ����� ������
        string name_s, age_s, pos_s, sal_s;

        cout << "������� ����� ���: ";
        getline(cin, name_s);

        cout << "������� ����� �������: ";
        getline(cin, age_s);
        if (prov(age_s) != 1) {
            cout << "������������ �������. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }

        cout << "������� ����� ���������: ";
        getline(cin, pos_s);

        cout << "������� ����� ��������: ";
        getline(cin, sal_s);
        if (prov(sal_s) < 1) {
            cout << "������������ ��������. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }

        // ���������� ������ � ����
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
            cout << "������ ���������� ������� ���������. ������� Enter ��� �����������...\n";
        }
        else {
            cout << "�� ������� �������� ������ ����������. ������� Enter ��� �����������...\n";
        }

        // ������� ��������
        delete updateStmt;
        delete result;
        delete checkStmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� ��������� ����������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//�������� ��������
void Delete_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string s;

        // ���� ID ����������
        cout << "\n������� ID ���������� ��� ��������: ";
        cin >> s;
        clearInputBuffer();

        // �������� ������������ �����
        if (prov(s) != 1) {
            cout << "������������ ���� ID. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }
        int employee_id = stoi(s);

        // �������� ������������� ����������
        PreparedStatement* checkStmt = con->prepareStatement(
            "SELECT id FROM employees WHERE id = ?"
        );
        checkStmt->setInt(1, employee_id);
        ResultSet* result = checkStmt->executeQuery();

        if (!result->next()) {
            cout << "��������� � ����� ID �� ������. ������� Enter ��� �����������...\n";
            delete result;
            delete checkStmt;
            cin.get();
            system("cls");
            return;
        }

        string full_name = result->getString("full_name");
        cout << "��������� ������: " << full_name << endl;

        // ������������� ��������
        cout << "�� ����� ������ ������� ����� ����������?\n1 - ��\n0 - ���\n";
        char choice;
        cin >> choice;
        clearInputBuffer();

        if (choice != '1') {
            system("cls");
            delete result;
            delete checkStmt;
            return;
        }

        // �������� �� ���� ������
        PreparedStatement* deleteStmt = con->prepareStatement(
            "DELETE FROM employees WHERE id = ?"
        );
        deleteStmt->setInt(1, employee_id);

        int affected_rows = deleteStmt->executeUpdate();

        if (affected_rows > 0) {
            cout << "��������� ������� ������. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");

        }
        else {
            cout << "�� ������� ������� ����������. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
        }

        // ������������ ��������
        delete deleteStmt;
        delete result;
        delete checkStmt;



    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� �������� ����������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//������ �������(�� ���� ����� + �� ���� ����������, ����� - 9 ��������)
void Filter_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        string a = "-1", b = "-1";
        char t, s;

        // ����� ���� ��� ����������
        cout << "������ ��:\n";
        cout << "1 - ID\n";
        cout << "2 - ��������\n";
        cout << "3 - ��������\n";
        cin >> t;
        clearInputBuffer();

        if (t < '1' || t > '3') {
            cout << "������������ ����. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }

        // ����� ���������
        cout << "> - ������\n< - ������\n# - � ��������\n";
        cin >> s;
        clearInputBuffer();

        // ����������� ����� ���� ��� SQL-�������
        string field_name;
        switch (t) {
        case '1': field_name = "id"; break;
        case '2': field_name = "age"; break;
        case '3': field_name = "salary"; break;
        }

        // ������������ ������� �������
        string where_clause;
        vector<double> params;

        switch (s) {
        case '>':
            cout << "������� ����������� ��������: ";
            cin >> a;
            clearInputBuffer();
            if (prov(a) < 1) {
                cout << "������������ ����. ������� Enter ��� �����������...\n";
                cin.get();
                system("cls");
                return;
            }
            where_clause = field_name + " > ?";
            params.push_back(stold(a));
            break;

        case '<':
            cout << "������� ������������ ��������: ";
            cin >> b;
            clearInputBuffer();
            if (prov(b) < 1) {
                cout << "������������ ����. ������� Enter ��� �����������...\n";
                cin.get();
                system("cls");
                return;
            }
            where_clause = field_name + " < ?";
            params.push_back(stold(b));
            break;

        case '#':
            cout << "������� ����������� ��������: ";
            cin >> a;
            clearInputBuffer();
            cout << "������� ������������ ��������: ";
            cin >> b;
            clearInputBuffer();
            if (prov(a) < 1 || prov(b) < 1) {
                cout << "������������ ����. ������� Enter ��� �����������...\n";
                cin.get();
                system("cls");
                return;
            }
            where_clause = field_name + " BETWEEN ? AND ?";
            params.push_back(stold(a));
            params.push_back(stold(b));
            break;

        default:
            cout << "������������ ����. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }

        // ������������ SQL-�������
        string sql = "SELECT * FROM employees WHERE " + where_clause;
        PreparedStatement* pstmt = con->prepareStatement(sql);

        // ����������� ����������
        for (size_t i = 0; i < params.size(); ++i) {
            pstmt->setDouble(i + 1, params[i]);
        }

        // ���������� �������
        ResultSet* res = pstmt->executeQuery();

        // ����� �����������
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
            cout << "���������� �� �������. ������� Enter ��� �����������...\n";
        }

        // ������� ��������
        delete res;
        delete pstmt;

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� ����������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//����� ��������
void Search_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        char t;
        string s;

        // ����� �������� ������
        cout << "����� ��:\n"
            << "1 - ID\n"
            << "2 - ���\n"
            << "3 - ��������\n"
            << "4 - ���������\n"
            << "5 - ��������\n";
        cin >> t;
        clearInputBuffer();

        if (t < '1' || t > '5') {
            cout << "������������ ����. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }

        // ���� �������� ��� ������
        cout << "\n������� �������� ��� ������:\n";
        cin >> s;
        clearInputBuffer();

        // ��������� �������� �����
        if (t == '1' || t == '3' || t == '5') {
            if (prov(s) != (t == '5' ? 2 : 1)) {
                cout << "������������ ����. ������� Enter ��� �����������...\n";
                cin.get();
                system("cls");
                return;
            }
        }

        // ������������ SQL-�������
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

        // ���������� �������
        unique_ptr<ResultSet> res(pstmt->executeQuery());

        // ����� �����������
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
            cout << "��������� �� ������. ������� Enter ��� �����������...\n";
        }

        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� ������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//���������� �������(�� ���� ����� + �� ����������� � ��������, ����� - 10 ��������)
void Sort_Employee() {
    try {
        Database db;
        Connection* con = db.getConnection();
        char x, t;

        // ����� ����������� ����������
        cout << "���������� �� �����������(>) ��� �� ��������(<)?\n";
        cin >> x;
        clearInputBuffer();

        if (x != '>' && x != '<') {
            cout << "������������ ����. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }

        // ����� ���� ��� ����������
        cout << "���������� ��:\n";
        cout << "1 - ID\n";
        cout << "2 - ���\n";
        cout << "3 - ��������\n";
        cout << "4 - ��������\n";
        cout << "5 - ���������\n";
        cin >> t;
        clearInputBuffer();

        // ����������� ���������� ����������
        string field_name;
        string order_direction = (x == '>') ? "ASC" : "DESC";

        switch (t) {
        case '1': field_name = "id"; break;
        case '2': field_name = "full_name"; break;
        case '3': field_name = "age"; break;
        case '4': field_name = "salary"; break;
        case '5': field_name = "position"; break;
        default:
            cout << "������������ ����. ������� Enter ��� �����������...\n";
            cin.get();
            system("cls");
            return;
        }

        // ������������ SQL-�������
        string sql = "SELECT * FROM employees ORDER BY " + field_name + " " + order_direction;
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery(sql);

        // ����� ��������������� ������
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

        // ������� ��������
        delete res;
        delete stmt;

        cout << "���������� �������������. ������� Enter ��� �����������...\n";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ SQL ��� ����������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}

//����� ���� ������� 
void Read_Employee() {
    try {
        system("cls");
        Database db;
        Connection* con = db.getConnection();

        // �������� � ���������� �������
        Statement* stmt = con->createStatement();
        ResultSet* res = stmt->executeQuery("SELECT * FROM employees");

        PrintEmployeeHeader();
        bool hasData = false;

        // ����� �����������
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
            cout << "������ ����������� ����\n";
        }

        // ������������ ��������
        delete res;
        delete stmt;

        cout << "\n������� Enter ��� �����������...";
        cin.get();
        system("cls");

    }
    catch (SQLException& e) {
        cerr << "������ ������ ������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
    catch (const exception& e) {
        cerr << "������: " << e.what() << endl;
        cin.get();
        system("cls");
    }
}