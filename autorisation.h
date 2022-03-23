#pragma once

#include <QString>
#include <tuple>
#include <string>
#include <vector>
#include <pqxx/pqxx>
#include <QTextStream>
#include "database_manager.h"
#include "hash.h"
#include "database_manager.h"
//#include "cionout.h"


bool Autorisation(DataBase& db);
bool Verification(const std::string& login, const std::string& master_pass, const std::string& pass_hash, const std::string& salt);
bool Registration(DataBase& db);
bool TableExist(DataBase& db);
bool CreateTable(DataBase& db);
std::pair<bool, std::vector<std::string>> UserExist(DataBase& db, const std::string& login);
