#-------------------------------------------------
#
# Project created by QtCreator 2016-09-20T20:45:11
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SecureMsgApp_Qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chatgroup.cpp \
    chatroom.cpp \
    creategroup.cpp \
    createaccount.cpp \
    server.cpp \
    manager.cpp \
    sslclient.cpp \
    chatclient.cpp \
    network.cpp


HEADERS  += mainwindow.h \
    library.h \
    chatgroup.h \
    chatroom.h \
    creategroup.h \
    createaccount.h \
    server.h \
    manager.h \
    sslclient.h \
    loadingscreen.h \
    chatclient.h \
    network.h

FORMS    += mainwindow.ui \
    chatgroup.ui \
    chatroom.ui \
    creategroup.ui \
    createaccount.ui

RESOURCES += \
    resource.qrc

CONFIG += c++11

INCLUDEPATH += C:\Users\Rollin\Desktop\cryptopp565

LIBS += C:\Users\Rollin\Desktop\cryptopp565\x64\Output\Release\cryptlib.lib
