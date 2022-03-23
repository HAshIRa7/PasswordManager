#include "password_manager.h"
/*
QTextStream cout(stdout);
QTextStream cin(stdin);
QTextStream cerr(stderr);
*/
QTextStream cerr(stderr);
QTextStream cin(stdin);
QTextStream cout(stdout);
PassMan::PassMan(DataBase& db1) {
    db = db1;
}

bool PassMan::Autorisation() {
    // getting login and master_password
    QString login, master_pass;
    cout << "Login: "; cout.flush();
    cin >> login;
    cout << "Master Password: "; cout.flush();
    cin >> master_pass;

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
        cerr << e.what() << "\n"; cerr.flush();
        return false;
    }

    if(res_req_1[0][0].c_str() == "1") {
        // нужная таблица есть
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
            cerr << e.what() << "\n"; cerr.flush();
            return false;
        }
        // создаем хэш пароля
    }
}

void PassMan::Menu() {
    cout << QString::fromStdString(std::string(20, '_')) << "\n"; cout.flush();
    cout << QString::fromStdString(std::string(8, '_')) << "Menu" <<
            QString::fromStdString(std::string(8, '_')) << "\n"; cout.flush();
    cout << "1. Create new password" << "\n"; cout.flush();
    cout << "2. Find all sites and apps connected to an email" << "\n"; cout.flush();
    cout << "3. Find a password for a site or app" << "\n"; cout.flush();
    cout << "4. Exit" << "\n"; cout.flush();
    cout << QString::fromStdString(std::string(20, '_'))<< "\n"; cout.flush();
    cout << ": "; cout.flush();
    QString answer;
    cin >> answer;
    if(answer == "1") {
        NewPassword();
    } else if(answer == "2") {
        FindByEmail();
    } else if(answer == "3") {
        FindPassword();
    } else if(answer == "4") {
        exit(0);
    } else {
        cout << "Choose one of the suggested options!\n";
        PassMan::Menu();
    }
}

void PassMan::NewPassword() {
    QString password, email, user_name, url, app_name;
    cout << "Enter user name: "; cout.flush();
    cin >> user_name;
    cout << "Enter email: "; cout.flush();
    cin >> email;
    cout << "Enter app_name: "; cout.flush();
    cin >> app_name;
    cout << "Enter URL: "; cout.flush();
    cin >> url;
    cout << "Enter password: "; cout.flush();
    cin >> password;
    PasswordItem pass(password.toStdString(), email.toStdString(), user_name.toStdString(),
                       url.toStdString(), app_name.toStdString());
    bool inserted = db.InsertPasswordItem(pass);
    if(!inserted) {
        cerr << "Create new password failed!\n"; cerr.flush();
    } else {
        cout << "Password successfully saved!\n"; cout.flush();
    }
}


void PassMan::FindByEmail() {
    QString email;
    cout << "Enter email: "; cout.flush();
    cin >> email;
    auto res = db.SelectEmailAll(email.toStdString()); // нужно прикрутить флаг успешно ли выполнился запрос БД
    if(!res.first) {
        cout << "Failed!\n"; cout.flush();
    } else {
        for(const auto& item : res.second) {
            cout << QString::fromStdString(item.first) << "\t" <<
                    QString::fromStdString(item.second) << "\n"; cout.flush();
        }
    }
}



void PassMan::FindPassword() {
    QString name;
    cout << "Enter url or app_name: "; cout.flush();
    cin >> name;
    auto res = db.FindPass(name.toStdString());
    bool found = res.first;
    if(found) {
        cout << "Password for " << name << ": " << QString::fromStdString(res.second) << "\n";
    }
}
