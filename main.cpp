#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <QClipboard>
#include <QString>
#include <QTextStream>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <pqxx/pqxx>

/*
QTextStream cout(stdout);
QTextStream cin(stdin);
QTextStream cerr1(stderr);
*/

#include "password_item.h"
#include "database_manager.h"
#include "encryption.h"
#include "password_manager.h"
#include "autorisation.h"
//#include "cionout.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /*
    const QString pass = "secret_key228sss";

    QClipboard *clipboard = a.clipboard();
    clipboard->setText(pass);
    */
    if(argc < 5) {
        throw std::runtime_error("To few command line arguments: expected 5");
        return 1;
    }

    DataBase db(argv[1], argv[2], argv[3], argv[4]);
    for(int i = 0; i < 3; i++) {
        bool autorisation = Autorisation(db);
        if(autorisation) {
            break;
        }
        if(i == 2) {
            QTextStream cout2(stdout);
            cout2 << "All 3 attempts are spent!\n"; cout2.flush();
            return 1;
        }
    }
    PassMan passman(db);
    while(true) {
        passman.Menu();
    }
    return a.exec();
}
