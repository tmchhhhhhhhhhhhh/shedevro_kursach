#include <iostream>
#include "database.h"
#include "handlers.h"
#include "utils.h"
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <Windows.h>
#include <iomanip>


using namespace std;

//Функция для чтения на русском
string read_utf8_line(const string& prompt) {
    std::wstring wbuffer(256, L'\0');
    DWORD charsRead;

    cout << prompt;

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    ReadConsoleW(hStdin, &wbuffer[0], static_cast<DWORD>(wbuffer.size()), &charsRead, nullptr);
    if (charsRead >= 2) {
        wbuffer[charsRead - 2] = L'\0'; // Удаление \r\n
    }

    // Перевод в UTF-8
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wbuffer.c_str(), -1, nullptr, 0, nullptr, nullptr);
    string result(utf8_len - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wbuffer.c_str(), -1, &result[0], utf8_len, nullptr, nullptr);
    return result;
}


int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
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
