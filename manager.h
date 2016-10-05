#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QTcpSocket>
#include "mainwindow.h"
#include "createaccount.h"
#include "chatgroup.h"
#include "chatroom.h"
#include "creategroup.h"
#include "server.h"

#include <QDebug> //debug purpose

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    ~Manager();

private:
    //pages
    MainWindow *startWindowUi;
    CreateAccount *createAccUi;
    Chatgroup *chatGrpUi;
    Chatroom *chatRmUi;
    CreateGroup *createGrpUi;
    //user data
    QString username;
    //token, private key
    //server
    Server * socServer;
    QTcpSocket *clientSocket;
    //setter
    void setUsername(QString user);
    const QString &getUsername();

signals:

public slots:
    void login(QString username, QString pass);
    void registerAcc();
    void showMain();
    void showChatGroup();
    void showChatRoom();
    void showCreateGroup();
    void hostServer();
    void displayMsg(QString);
    void chatRoomConnected();
    void chatRoomDisconnected();
    void chatRoomReadyRead();
    void chatRoomSendMsg(QString);
};

#endif // MANAGER_H
