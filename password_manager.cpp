#include "password_manager.h"

#ifndef STREAM
    #define STREAM
    #include <QTextStream>
    QTextStream cout6(stdout);
    QTextStream cin6(stdin);
    QTextStream cerr6(stderr);
#endif

PassMan::PassMan(DataBase& db1) {
    this->db = db1;
}

bool PassMan::Autorisation() {
    // getting login and master_password
    QString login, master_pass;
    cout6 << "Login: "; cout6.flush();
    cin6 >> login;
    cout6 << "Master Password: "; cout6.flush();
    cin6 >> master_pass;

    // проверяем наличие таблицы с хэшем пароля.
    // если таковой нет, то создаём новую, регистрируя юзера
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
        cerr6 << e.what() << "\n"; cerr6.flush();
        //cerr6 << "dfdfd\n"; cerr6.flush();
        return false;
    }

    //cout6 << res_req_1[0][0].c_str() << "\n"; cout6.flush();
    if(strcmp(res_req_1[0][0].c_str(), "t") == 0) {
        // нужная таблица есть
        auto res = UserExist(db, login.toStdString());
        if(res.first) {
            std::string pass_hash, salt;
            pass_hash = res.second[1];
            salt = res.second[2];
            bool ver = Verification(login.toStdString(), master_pass.toStdString(), pass_hash, salt);
            if(!ver) {
                cout6 << "Verification failed!\n"; cout6.flush();
            } else {
                return true;
            }
        } else {
            return Registration(db);
        }
    } else {
        // нужной таблицы нет
        std::stringstream ss_req_2;
        ss_req_2 << "CREATE TABLE autorisation (login varchar (300),\
                     password varchar (300))";
        pqxx::result res_req_2;
        try {
            pqxx::connection C(db.GetDBconnectionInfo());
            pqxx::work w(C);
            res_req_2 = w.exec(ss_req_2.str());
            w.commit();
            C.disconnect();
        } catch(const std::exception& e) {
            cerr6 << e.what() << "\n"; cerr6.flush();
            //cerr6 << "dfdfd\n"; cerr6.flush();
            return false;
        }
        // создаем хэш пароля
        std::string salt = GenerateSalt(master_pass.size() / 2);
        std::string pass_hash = GetHash(master_pass.toStdString(), salt);

        // занесем пользователя в БД
        std::stringstream ss_req_3;
        ss_req_3 << "INSERT INTO autorisation VALUES ('" << login.toStdString() << "', '"
                 << pass_hash << "', '" << salt << "');";

        pqxx::result res_req_3;
        try {
            pqxx::connection C(db.GetDBconnectionInfo());
            pqxx::work w(C);
            res_req_3 = w.exec(ss_req_3.str());
            w.commit();
            C.disconnect();
        } catch(const std::exception& e) {
            cerr6 << e.what() << "\n"; cerr6.flush();
            //cerr6 << "dfdfd\n"; cerr6.flush();
            return false;
        }
        return Autorisation();
    }
    return false;
}

void PassMan::Menu() {
    cout6 << QString::fromStdString(std::string(20, '_')) << "\n"; cout6.flush();
    cout6 << QString::fromStdString(std::string(8, '_')) << "Menu" <<
            QString::fromStdString(std::string(8, '_')) << "\n"; cout6.flush();
    cout6 << "1. Create new password" << "\n"; cout6.flush();
    cout6 << "2. Find all sites and apps connected to an email" << "\n"; cout6.flush();
    cout6 << "3. Find a password for a site or app" << "\n"; cout6.flush();
    cout6 << "4. Exit" << "\n"; cout6.flush();
    cout6 << QString::fromStdString(std::string(20, '_'))<< "\n"; cout6.flush();
    cout6 << ": "; cout6.flush();
    QString answer;
    cin6 >> answer;
    if(answer == "1") {
        NewPassword();
    } else if(answer == "2") {
        FindByEmail();
    } else if(answer == "3") {
        FindPassword();
    } else if(answer == "4") {
        return;
    } else {
        cout6 << "Choose one of the suggested options!\n";
    }
    PassMan::Menu();
}

void PassMan::NewPassword() {
    QString password, email, user_name, url, app_name;
    cout6 << "Enter user name: "; cout6.flush();
    cin6 >> user_name;
    cout6 << "Enter email: "; cout6.flush();
    cin6 >> email;
    cout6 << "Enter app_name: "; cout6.flush();
    cin6 >> app_name;
    cout6 << "Enter URL: "; cout6.flush();
    cin6 >> url;
    cout6 << "Enter password: "; cout6.flush();
    cin6 >> password;
    PasswordItem pass(password.toStdString(), email.toStdString(), user_name.toStdString(),
                       url.toStdString(), app_name.toStdString());
    bool inserted = db.InsertPasswordItem(pass);
    if(!inserted) {
        cerr6 << "Create new password failed!\n"; cerr6.flush();
    } else {
        cout6 << "Password successfully saved!\n"; cout6.flush();
    }
}


void PassMan::FindByEmail() {
    QString email;
    cout6 << "Enter email: "; cout6.flush();
    cin6 >> email;
    auto res = db.SelectEmailAll(email.toStdString()); // нужно прикрутить флаг успешно ли выполнился запрос БД
    if(!res.first) {
        cout6 << "Failed!\n"; cout6.flush();
    } else {
        for(const auto& item : res.second) {
            cout6 << QString::fromStdString(item.first) << "\t" <<
                    QString::fromStdString(item.second) << "\n"; cout6.flush();
        }
    }
}



void PassMan::FindPassword() {
    QString name;
    cout6 << "Enter url or app_name: "; cout6.flush();
    cin6 >> name;
    auto res = db.FindPass(name.toStdString());
    bool found = res.first;
    if(found) {
        cout6 << "Password for " << name << ": " << QString::fromStdString(res.second) << "\n";
        cout6.flush();
    }
}
