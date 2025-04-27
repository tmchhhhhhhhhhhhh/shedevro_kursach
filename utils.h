#ifndef UTILS_H
#define UTILS_H

#include <string>

const int ID_WIDTH = 6;
const int NAME_WIDTH = 25;
const int AGE_WIDTH = 10;
const int POS_WIDTH = 20;
const int SAL_WIDTH = 12;
const int LOGIN_WIDTH = 20;
const int PASS_WIDTH = 20;


struct Employee {
    int id;
    std::string full_name;
    int age;
    std::string Position;
    double salary;
};

std::string sha256(const std::string& input);
std::string Get_Password();
int prov(std::string s);
void clearInputBuffer();
std::string read_utf8_line(const std::string& prompt);



#endif // UTILS_H
