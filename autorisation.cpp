#include "autorisation.h"


/*
QTextStream cout(stdout);
QTextStream cin(stdin);
QTextStream cerr(stderr);
*/
QTextStream cin(stdin);
QTextStream cout(stdout);
QTextStream cerr(stdout);
bool TableExist(DataBase& db) {
    std::stringstream ss_req_1;
    ss_req_1 << "SELECT EXISTS (SELECT 1 FROM information_schema.columns\
                WHERE table_name = 'autorisation');";
    pqxx::result res_req_1;
    try {
        pqxx::connection C(db.GetDBconnectionInfo());
        pqxx::work w(C);
        res_req_1 = w.exec(ss_req_1.str());
        w.commit();
        C.disconnect();
    } catch(const std::exception& e) {
        cerr << e.what() << "\n"; cerr.flush();
        return false;
    }
    return res_req_1[0][0].c_str() == "1";
}


bool CreateTable(DataBase& db) {
    std::stringstream ss_req_2;
    ss_req_2 << "CREATE TABLE autorisation (login varchar (300) NOT NULL,\
                 password varchar (300) NOT NULL, salt varchar (300) NOT NULL UNIQUE)";
    pqxx::result res_req_2;
    try {
        pqxx::connection C(db.GetDBconnectionInfo());
        pqxx::work w(C);
        res_req_2 = w.exec(ss_req_2.str());
        w.commit();
        C.disconnect();
    } catch(const std::exception& e) {
        cerr << e.what() << "\n"; cerr.flush();
        return false;
    }
    return true;
}

std::pair<bool, std::vector<std::string>> UserExist(DataBase& db, const std::string& login) {
    // if exists, returns info: login, pass_hash, salt
    std::stringstream ss_req;
    ss_req << "SELECT * FROM autorisation WHERE login = '" << login << "');";
    pqxx::result res_req;
    try {
        pqxx::connection C(db.GetDBconnectionInfo());
        pqxx::work w(C);
        res_req = w.exec(ss_req.str());
        w.commit();
        C.disconnect();
    } catch(const std::exception& e) {
        cerr << e.what() << "\n"; cerr.flush();
        std::vector<std::string> v = {"", "", ""};
        return std::make_pair(false, v);
    }

    if(res_req.size() > 0) {
        std::string pass_hash, salt;
        pass_hash = res_req[0][1].c_str();
        salt = res_req[0][2].c_str();
        std::vector<std::string> v = {login, pass_hash, salt};
        return std::make_pair(true, v);
    }
    std::vector<std::string> v2 = {"", "", ""};
    return std::make_pair(false, v2);
}


bool Verification(const std::string& login, const std::string& master_pass,
                  const std::string& pass_hash, const std::string& salt) {
    return pass_hash == GetHash(master_pass, salt);
}

bool Registration(DataBase& db) {
    QString master_pass1, master_pass2, login;
    cout << "Enter login: "; cout.flush();
    cin >> login;
    cout << "Enter Master password: "; cout.flush();
    cin >> master_pass1;
    cout << "Repeat Master password: "; cout.flush();
    cin >> master_pass2;
    if(master_pass1 != master_pass2) {
        cout << "the entered passwords do not match!\n"; cout.flush();
        return false;
    }

    auto hash1 = GetHashWithSalt(master_pass1.toStdString());
    if(std::get<0>(hash1)) {
        std::string pass_hash = std::get<1>(hash1);
        std::string salt = std::get<2>(hash1);
        // запись данных в БД
        std::stringstream ss_req;
        ss_req << "INSERT INTO autorisation VALUES ('" << login.toStdString() << "', '"
               << pass_hash << "', '" << salt << "');";
        pqxx::result res_req;
        try {
            pqxx::connection C(db.GetDBconnectionInfo());
            pqxx::work w(C);
            res_req = w.exec(ss_req.str());
            w.commit();
            C.disconnect();
        } catch(const std::exception& e) {
            cerr << e.what() << "\n"; cerr.flush();
            return false;
        }
        return true;
    }
    return false;
}


bool Autorisation(DataBase& db) {

    if(TableExist(db)) {
        QString login, master_pass;
        cout << "Login: "; cout.flush();
        cin >> login;
        cout << "Master password: "; cout.flush();
        cin >> master_pass;
        auto user_info = UserExist(db, login.toStdString());

        if(user_info.first) {
            bool ver = Verification(user_info.second[0], login.toStdString(), user_info.second[1], user_info.second[2]);
            return ver;
        } else {
            // user with login doesn't exist
            return Registration(db);
        }
    } else {
        if(CreateTable(db)) {
            return Registration(db);
        }
    }
    return true;
}
