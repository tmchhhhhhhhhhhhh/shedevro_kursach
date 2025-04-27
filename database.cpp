#include "database.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>


Database::Database() {
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
    con->setSchema("employee_db");
    con->createStatement()->execute("SET NAMES 'utf8mb4'");
}

Database::~Database() {
    delete con;
}

sql::Connection* Database::getConnection() {
    return con;
}
