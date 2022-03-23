#pragma once
#include "database_manager.h"
#include <QString>
#include <iostream>
#include <QTextStream>


class PassMan {
public:
    PassMan(DataBase& db);
    void Menu();
    void NewPassword();
    void FindByEmail();
    void FindPassword();
    bool Autorisation();
private:
    DataBase db;
};
