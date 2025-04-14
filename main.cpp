#include <iostream>
#include "database.h"
#include "handlers.h"
#include "utils.h"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>


using namespace std;

int main() {
    setlocale(LC_ALL, "Rus");
    FirstRunSetup();
    char choice = '1';
    while (choice != '0') {
        cout << "---ИС ДЛЯ УЧЁТА КАДРОВ НА ПРЕДПРИЯТИИ---\n";
        cout << "2 - Регистрация в системе\n";
        cout << "1 - Вход в систему\n";
        cout << "0 - Выход из программы\n";
        cout << "Введите номер операции:\n";
        cin >> choice;
        clearInputBuffer();
        system("cls");
        if (choice == '1') {
            Enter();
        }
        else if (choice == '2') {
            User_Registration();
        }
    }
    return 0;
}
