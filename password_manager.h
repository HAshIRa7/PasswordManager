#pragma once
#include "database_manager.h"
#include <QString>
#include <iostream>
#include <QTextStream>
#include <tuple>
#include "autorisation.h"

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
