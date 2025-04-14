#ifndef HANDLERS_H
#define HANDLERS_H

void FirstRunSetup();
void Enter();
void Menu(int user_id, bool is_admin);
void User_Registration();

void Add_Employee();
void Change_Employee();
void Delete_Employee();
void Read_Employee();
void Search_Employee();
void Filter_Employee();
void Sort_Employee();

void Add_User();
void Delete_User();
void Read_User();
void Verify_User();
void PrintEmployeeHeader();
void PrintEmployeeRow(const struct Employee& emp);
void PrintUserHeader();

#endif // HANDLERS_H
