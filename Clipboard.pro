QT       += core gui
QT       += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/postgresql
LIBS += -L/usr/include/postgresql -lpqxx

INCLUDEPATH += /usr/include/crypto++
LIBS += -L/usr/include/crypto++ -lcryptopp

SOURCES += \
    autorisation.cpp \
    cionout.cpp \
    database_manager.cpp \
    encryption.cpp \
    hash.cpp \
    main.cpp \
    mainwindow.cpp \
    password_item.cpp \
    password_manager.cpp

HEADERS += \
    autorisation.h \
    cionout.h \
    database_manager.h \
    encryption.h \
    hash.h \
    mainwindow.h \
    password_item.h \
    password_manager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
