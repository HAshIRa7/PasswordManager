/*
    g++ -Wall -Wextra main.cpp -o main.exe -lpqxx
*/
/*
    g++ -c database_manager.cpp -lpqxx
    g++ -c password_item.cpp -lpqxx
    g++ -c main.cpp -lpqxx
    g++ database_manager.o password_item.o main.o -o main.exe -lpqxx
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <string.h>
#include <pqxx/pqxx>

#include "password_item.h"
#include "database_manager.h"
/*
 password | email | user_name | url | app_name
*/



void NewPassword(DataBase& db) {
    std::string password, email, user_name, url, app_name;
    std::cout << "Enter user name: ";
    std::cin >> user_name;
    std::cout << "Enter email: ";
    std::cin >> email;
    std::cout << "Enter app_name: ";
    std::cin >> app_name;
    std::cout << "Enter URL: ";
    std::cin >> url;
    std::cout << "Enter password: ";
    std::cin >> password;
    PasswordItem pass(password, email, user_name, url, app_name);
    bool inserted = db.InsertPasswordItem(pass);
    if(!inserted) {
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
    auto res = db.FindPass(name);
    bool found = res.first;
    if(found) {
        std::cout << "Password for " << name << ": " << res.second << std::endl;
    }
}


void Menu(DataBase& db) {
    std::cout << std::string(20, '_') << std::endl;
    std::cout << std::string(8, '_') << "Menu" << std::string(8, '_') << std::endl;
    std::cout << "1. Create new password" << std::endl;
    std::cout << "2. Find all sites and apps connected to an email" << std::endl;
    std::cout << "3. Find a password for a site or app" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << std::string(20, '_') << std::endl;
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
    DataBase db(argv[1], argv[2], argv[3], argv[4]);
    std::string request;
    while(true) {
        Menu(db);
    }
    return 0;
}