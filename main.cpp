/*
    g++ -Wall -Wextra main.cpp -o main.exe -lpqxx
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <string.h>
#include <pqxx/pqxx>
/*
 password | email | user_name | url | app_name
*/
enum class PassField {
    PASSWORD,
    EMAIL,
    USER_NAME,
    URL,
    APP_NAME
};


class PasswordItem {
public:
    PasswordItem(std::string password, std::string email, std::string user_name, 
        std::string url, std::string app_name) {
        _password = password;
        _email = email;
        _user_name = user_name;
        _url = url;
        _app_name = app_name;
    }
    std::map<PassField, std::string> GetPasswordItem() {
        std::map<PassField, std::string> res = {
            {PassField::PASSWORD, _password},
            {PassField::EMAIL, _email},
            {PassField::USER_NAME, _user_name},
            {PassField::URL, _url},
            {PassField::APP_NAME, _app_name}
        };
        return res;
    }

private:
    std::string _password;
    std::string _email;
    std::string _user_name;
    std::string _url;
    std::string _app_name;
};


class DataBase {
public:
    DataBase(const std::string& db_name, const std::string& table_name, const std::string password, const std::string user_name);
    bool InsertPasswordItem(PasswordItem& item); // запрос INSERT в БД
    std::pair<bool, std::vector<std::pair<std::string, std::string>>>
        SelectEmailAll(const std::string& email); // Finds all sites and apps connected to an email. Возвращает пары <url, app_name>
    std::pair<bool, std::string> FindPass(const std::string& name); // ищет пароль по name: это либо url, либо app_name
    std::string GetDBName();
    std::string GetTableName();
private:
    std::string _password;
    std::string _user_name;
    std::string _db_name;
    std::string _table_name;
};

std::string DataBase::GetDBName() {
    return _db_name;
}

std::string DataBase::GetTableName() {
    return _table_name;
}

DataBase::DataBase(const std::string& db_name, const std::string& table_name, 
    const std::string password, const std::string user_name) {
    _password = password;
    _user_name = user_name;
    _db_name = db_name;
    _table_name = table_name;
}


bool DataBase::InsertPasswordItem(PasswordItem& item) {
    // database connect info
    std::stringstream ss_conn_info;
    ss_conn_info << "dbname = " << _db_name << " user = " << _user_name << " password = " << _password;
    std::string conn_info = ss_conn_info.str();

    // making request
    auto pass = item.GetPasswordItem();
    std::stringstream ss_req;
    ss_req << "INSERT INTO " << _table_name << " (password, email, user_name, url, app_name) VALUES ("
        << "'" << pass[PassField::PASSWORD]     << "', " 
        << "'" << pass[PassField::EMAIL]        << "', "
        << "'" << pass[PassField::USER_NAME]    << "', "
        << "'" << pass[PassField::URL]          << "', "
        << "'" << pass[PassField::APP_NAME]     << "');";
    std::string req = ss_req.str();

    pqxx::result res;
    try {
        pqxx::connection C(conn_info);
        pqxx::work w(C);
        res = w.exec(req);
    } catch(std::exception const& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}


void NewPassword(DataBase& db) {
    std::string password, email, user_name, url, app_name;
    std::cout << "Enter user name: ";
    std::cin >> user_name;
    std::cout << std::endl;
    std::cout << "Enter email: ";
    std::cin >> email;
    std::cout << std::endl;
    std::cout << "Enter app_name: ";
    std::cin >> app_name;
    std::cout << std::endl;
    std::cout << "Enter URL: ";
    std::cin >> url;
    std::cout << std::endl;
    std::cout << "Enter password: ";
    std::cin >> password;
    std::cout << std::endl;
    PasswordItem pass(password, email, user_name, url, app_name);
    bool inserted = db.InsertPasswordItem(pass);
    if(!inserted) {
        /*
            хорошо бы сюда прокидывать сообщение об ошибке из самой БД
        */
        std::cerr << "Create new password failed!" << std::endl;
    } else {
        std::cout << "Password successfully saved!" << std::endl;
    }
}


void FindByEmail(DataBase& db) {
    std::string email;
    std::cout << "Enter email: ";
    std::cin >> email;
    auto res = db.SelectEmailAll(email); // нужно прикрутить флаг успешно ли выполнился запрос БД
    if(!res.first) {
        std::cout << "Failed!" << std::endl;
    } else {
        for(const auto& item : res.second) {
            std::cout << item.first << "\t" << item.second << std::endl;
        }
    }
}

void FindPassword(DataBase& db) {
    std::string name;
    std::cout << "Enter url or app_name: ";
    std::cin >> name;
    std::cout << std::endl;

    auto res = db.FindPass(name);
    bool found = res.first;
    if(!found) {
        std::cout << "No password or smth went wrong!" << std::endl;
    } else {
        std::cout << "Password: " << res.second << std::endl;
    }
}


void Menu(DataBase& db) {
    std::cout << '-'*20 << std::endl;
    std::cout << '-'*8 << "Menu" << '-'*8 << std::endl;
    std::cout << "1. Create new password" << std::endl;
    std::cout << "2. Find all sites and apps connected to an email" << std::endl;
    std::cout << "3. Find a password for a site or app" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << '-'*20 << std::endl;
    std::cout << ": ";
    std::string answer;
    std::cin >> answer;
    if(answer == "1") {
        NewPassword(db);
    } else if(answer == "2") {
        FindByEmail(db);
    } else if(answer == "3") {
        FindPassword(db);
    } else if(answer == "4") {
        exit(0);
    } else {
        std::cout << "Choose one of the suggested options" << std::endl;
        Menu(db);
    }
}


int main(int argc, char* argv[]) {
    // argv: db_name, table_name, user_name (database), password (database)
    if(argc < 5) {
        throw std::runtime_error("To few command line arguments: expected 5");
        return 1;
    } 
    DataBase db(std::string(argv[1], sizeof(argv[1])), std::string(argv[2], sizeof(argv[2])), 
        std::string(argv[3], sizeof(argv[3])), std::string(argv[4], sizeof(argv[4])));
    std::string request;
    while(true) {
        Menu(db);
    }
    return 0;
}