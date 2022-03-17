#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <libpq-fe.h>


/*
"INSERT INTO passwords (password, email, user_name, url, app_name) VALUES (";
*/


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
    DataBase(const std::string& db_name, const std::string& table_name);
    bool InsertPasswordItem(PasswordItem& item); // запрос INSERT в БД
    std::pair<bool, std::vector<std::pair<std::string, std::string>>>
        SelectEmailAll(const std::string& email); // Finds all sites and apps connected to an email. Возвращает пары <url, app_name>
    std::pair<bool, std::string> FindPass(const std::string& name); // ищет пароль по name: это либо url, либо app_name
    std::string GetDBName();
    std::string GetTableName();
private:
    std::string _db_name;
    std::string _table_name;
};

std::string DataBase::GetDBName() {
    return _db_name;
}

std::string DataBase::GetTableName() {
    return _table_name;
}

DataBase::DataBase(const std::string& db_name, const std::string& table_name) {
    _db_name = db_name;
    _table_name = table_name;
}


bool DataBase::InsertPasswordItem(PasswordItem& item) {
    const std::string s = "dbname = ";
    const char* conn_info = (s + _db_name).c_str();
    auto conn = PQconnectdb(conn_info);
    if(PQstatus(conn) == CONNECTION_BAD) {
        PQfinish(conn);
        return false;
    }
    // формирование запроса
    std::string req = "INSERT INTO " + _table_name + " (password, email,\
        user_name, url, app_name) VALUES (";
    //password | email | user_name | url | app_name
    auto pass = item.GetPasswordItem();
    req += "'" +  pass[PassField::PASSWORD] + "', ";
    req += "'" +  pass[PassField::EMAIL] + "', ";
    req += "'" +  pass[PassField::USER_NAME] + "', ";
    req += "'" +  pass[PassField::URL] + "', ";
    req += "'" +  pass[PassField::APP_NAME] + ");";

    auto res = PQexec(conn, req.c_str());
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    PQclear(res);
    PQfinish(conn);
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


int main() {
    DataBase db("passwords", "passwords");
    std::string request;
    while(true) {
        Menu(db);
    }
    return 0;
}