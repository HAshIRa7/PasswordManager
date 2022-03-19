#include "database_manager.h"

DataBase::DataBase(const std::string& db_name, const std::string& table_name, 
    const std::string user_name, const std::string password) {
        //db_name, table_name, user_name (database), password (database)
    _password = password;
    _user_name = user_name;
    _db_name = db_name;
    _table_name = table_name;
}


std::pair<bool, std::string> DataBase::FindPass(const std::string& name) {
    /*
        Searches for a password by the 'name' in the url and app_name fields
        for current user
    */
    // making SELECT request string
    std::stringstream ss_req;
    ss_req << "SELECT * FROM " << _table_name << " WHERE user_name = '" << _user_name << "' AND url = '" << name 
    << "' OR user_name = '" << _user_name << "' AND app_name = '" << name << "';";
    //std::cout << "'" << name << "'" << std::endl;
    // result to return
    std::pair<bool, std::string> res_p;

    //connection to database
    try {
        pqxx::connection C(GetDBconnectionInfo());
        pqxx::work w(C);
        pqxx::result res = w.exec(ss_req.str());
        w.commit();
        if(res.size() == 0) {
            std::cout << "There are no any passwords found by '" << name << "'!" << std::endl;
            res_p.first = false;
            res_p.second = "";
            return res_p;
        }
        res_p = std::make_pair(true, res[0][0].c_str());
        C.disconnect();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        res_p.first = false;
        res_p.second = "";
        return res_p;
    }
    return res_p;
}


std::pair<bool, std::vector<std::pair<std::string, std::string>>>
        DataBase::SelectEmailAll(const std::string& email) {
        /*
            / Finds all sites and apps connected to an email. Returns pairs <url, app_name>
        */
    // making SELECT request string
    std::stringstream ss_req;
    ss_req << "SELECT * FROM " << _table_name << " WHERE email = '" << email << "' AND user_name = '" << _user_name << "';";


    std::vector<std::pair<std::string, std::string>> result_v;
    // connection to database
    try {
        pqxx::connection C(GetDBconnectionInfo());
        pqxx::work w(C);
        pqxx::result res = w.exec(ss_req.str());
        w.commit();
        if(res.size() == 0) {
            std::cout << "There are no any passwords in " << _table_name << "!" << std::endl;
            std::vector<std::pair<std::string, std::string>> v = {std::make_pair("", "")};
            return std::make_pair(false, v);
        }
        const char* e = "email";
        const char* u = "url";
        for(const auto& row : res) {
            std::pair<std::string, std::string> p = std::make_pair(row[3].c_str(), row[4].c_str());
            result_v.push_back(p);
        }
        C.disconnect();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::vector<std::pair<std::string, std::string>> v = {std::make_pair("", "")};
        return std::make_pair(false, v);
    }
    return std::make_pair(true, result_v);
}


bool DataBase::InsertPasswordItem(PasswordItem& item) {
    /*
        Creates the _table_name table if it doesn't already exist 
        and executes a password insert query.
    */
    // database connect info
    std::string conn_info = GetDBconnectionInfo();

    // making request string
    auto pass = item.GetPasswordItem();
    std::stringstream ss_req;
    ss_req << "INSERT INTO " << _table_name << " (password, email, user_name, url, app_name) VALUES ("
        << "'" << pass[PassField::PASSWORD]     << "', " 
        << "'" << pass[PassField::EMAIL]        << "', "
        << "'" << pass[PassField::USER_NAME]    << "', "
        << "'" << pass[PassField::URL]          << "', "
        << "'" << pass[PassField::APP_NAME]     << "');";
    std::string request_str = ss_req.str();

    // create table request string
    std::stringstream ss_table;
    ss_table << "CREATE TABLE IF NOT EXISTS " << _table_name << "( " << "password varchar (500) NOT NULL UNIQUE, "
    << "email varchar (100) NOT NULL, " << "user_name varchar (100) NOT NULL, "
    << "url varchar (100) NOT NULL, " << "app_name varchar (100) NOT NULL);";

    pqxx::result res;
    try {
        pqxx::connection C(conn_info);
        if(!C.is_open()) {
            std::cerr << "Failed connection to database!" << std::endl;
            return false;
        }
        pqxx::work w(C);
        // create table if it doesn't exist
        w.exec(ss_table.str());
        // executing INSERT request
        w.exec(request_str);
        w.commit();
        C.disconnect();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}

std::string DataBase::GetDBconnectionInfo() {
    std::stringstream ss;
    ss << "user = " << _user_name << " password = " << _password << " dbname = " << _db_name;
    return ss.str();
}

std::string DataBase::GetDBName() {
    return _db_name;
}

std::string DataBase::GetTableName() {
    return _table_name;
}