#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <libpq-fe.h>

class Password {
public:
    std::string password;
    std::string name;
};

class InsertPasswordQuery {
public:
    InsertPasswordQuery();
    InsertPasswordQuery(std::string& pass, std::string email, std::string user_name, 
        std::string url, std::string app_name) {
        _password = pass;
        _email = email;
        _user_name = user_name;
        _url = url;
        _app_name = app_name;
    }

    std::string GetInsertQuery() {
        std::string query = "INSERT INTO passwords (password, email,\
                         user_name, url, app_name) VALUES (";
        query += "'" + _password + "', ";
        query += "'" + _email + "', "; 
        query += "'" + _user_name + "', ";
        query += "'" + _url + "', ";
        query += "'" + _app_name + "');";
        return query;
    }

private:
    std::string _password;
    std::string _email;
    std::string _user_name;
    std::string _url;
    std::string _app_name;    
};


class PassMan {
public:
    bool SetPassword(const std::string& password, const std::string& name);
    std::pair<bool, std::string> GetPassword(const std::string& name);
    bool DelPassword(const std::string& name);
    std::pair<std::string, std::string> 
            ResetPassword(const std::string& name, const std::string& password);
    std::map<std::string, std::string> GetAllPasswords();
    bool DB_insert_pass(const InsertPasswordQuery& q);
private:
    std::map<std::string, std::string> passwords;
};


std::map<std::string, std::string> PassMan::GetAllPasswords() {
    return passwords;
}

bool PassMan::DB_insert_pass(const InsertPasswordQuery& q) {
    const char* conninfo;
    conninfo = "dbname = passwords";
    auto conn = PQconnectdb(conninfo);
    if(PQstatus(conn) == CONNECTION_BAD) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return false;
    }
    std::string query_str = q.GetInsertQuery();
    auto res = PQexec(conn, query_str.c_str());
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "INSERT query faile: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return false;
    }
    PQclear(res);
    PQfinish(conn);
    return true;
}


std::pair<std::string, std::string>  
        PassMan::ResetPassword(const std::string& name, const std::string& password) {
            auto old = GetPassword(name);
            if(!old.first) {
                std::cerr << "Reset failed!" << std::endl;
                return std::make_pair("", "");
            }
            bool added = SetPassword(name, password);
            if(!added) {
                std::cerr << "Set failed!" << std::endl;
                return std::make_pair("", "");
            }
            return std::make_pair(old.second, password);
}

bool PassMan::DelPassword(const std::string& name) {
    auto it = passwords.find(name);
    if(it == end(passwords)) {
        //std::cerr << "No password with name '" << name << "'" << std::endl;
        return false;
    }
    passwords.erase(name);
    return true;
}


bool PassMan::SetPassword(const std::string& name, const std::string& password) {
    //bool inserted = passwords.insert(std::make_pair(name, password)).second;
    passwords[name] = password;
    return true;
}

std::pair<bool, std::string> PassMan::GetPassword(const std::string& name) {
    auto it = passwords.find(name);
    if(it == end(passwords)) {
        // password with 'name' doesn't exist
        return std::make_pair(false, "");
    }
    return std::make_pair(true, passwords[name]);
}

std::ostream& operator<<(std::ostream& os, PassMan& passman) {
    if(passman.GetAllPasswords().empty()) {
        os << "There are no any passwords!" << std::endl;
        return os;
    }
    for(const auto& item : passman.GetAllPasswords()) {
        os << "name: '" << item.first << "'\tpassword: '" << item.second << "'" << std::endl;
    }
    return os;
}

enum class Request {
    PRINT,
    SET,
    RESET,
    DEL,
    QUIT,
    UNKNOWN
};


Request GetRequestType(const std::string& s) {
    if(s == "PRINT") {
        return Request::PRINT;
    } else if(s == "SET") {
        return Request::SET;
    } else if(s == "DEL") {
        return Request::DEL;
    } else if(s == "QUIT") {
        return Request::QUIT;
    } else if(s == "RESET") {
        return Request::RESET;
    }
    return Request::UNKNOWN;
}


void HadleRequest(const Request& req, PassMan& passman) {
    if(req == Request::PRINT) {
        std::cout << passman;
    } else if(req == Request::SET) {
        std::string name, password;
        std::cin >> name >> password;
        bool added = passman.SetPassword(name, password);
        if(!added) {
            std::cerr << "Password add failed!" << std::endl;
        }
    } else if(req == Request::DEL) {
        std::string name;
        std::cin >> name;
        bool deleted = passman.DelPassword(name);
        if(!deleted) {
            std::cerr << "Password hasn't been deleted!" << std::endl;
        }
    } else if(req == Request::QUIT) {
        exit(0);
    } else if(req == Request::RESET) {
        std::string name, password;
        std::cin >> name >> password;
        auto res = passman.ResetPassword(name, password);
        std::cout << "Password to "<< name << "changed from '" << res.first 
                << "' to " << password << "'" << std::endl; 
    } 
    else if(req == Request::UNKNOWN){
        std::cerr << "Unknown command!" << std::endl;
    }
}


int main() {
    PassMan passman;
    std::string request;
    while(true) {
        std::cin >> request;
        HadleRequest(GetRequestType(request), passman);
    }
    return 0;
}