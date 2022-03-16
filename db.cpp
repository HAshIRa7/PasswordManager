#include <iostream>
#include <vector>
#include <libpq-fe.h>
// g++ -I/usr/include/postgresql db.cpp -o db.exe -lpq


std::string GetInsertQuery(std::vector<std::string>& values) {
    std::string query = "INSERT INTO passwords (password, email,\
                         user_name, url, app_name) VALUES (";
    size_t N = values.size();
    for(size_t i = 0; i != N; i++) {
        query += "'" + values[i] + "'";
        if(i == N-1) {
            query += ");";
        } else {
            query += ", ";
        }
    }
    return query;
}


int main() {
    const char* conninfo;
    conninfo = "dbname = passwords";
    auto conn = PQconnectdb(conninfo);
    if(PQstatus(conn) == CONNECTION_BAD) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        exit(1);
    } else {
        std::cout << "Connection succeded!" << std::endl;
    }
    char* dbname = PQdb(conn);
    std::cout << dbname << std::endl;

    std::string password = "0000";
    std::string email = "user@mail.com";
    std::string user_name = "superuser";
    std::string url = "http://rubtsov.su/";
    std::string app_name = "rubtsov";
    std::vector<std::string> values = {password, email, user_name, url, app_name};
    std::string query = GetInsertQuery(values);
    auto res = PQexec(conn, query.c_str());
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "INSERT query faile: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        exit(1);
    } else {
        std::cout << "Done!" << std::endl;
    }
    PQclear(res);
    PQfinish(conn);
    return 0;
}