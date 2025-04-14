#ifndef DATABASE_H
#define DATABASE_H

#include <cppconn/driver.h>
#include <cppconn/connection.h>

class Database {
private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

public:
    Database();
    ~Database();

    sql::Connection* getConnection();
};

#endif // DATABASE_H
