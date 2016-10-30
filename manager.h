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
#include "sslclient.h"

#include <QDebug> //debug purpose

#include "loadingscreen.h" //loading screen

#define TIMEOUT 5000

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
    //user data, converting to struct
    QString username;
    QString token;
    //token, private key
    //server
    Server *socServer;
    QTcpSocket *clientSocket;
    //HTTPS client
    SSLClient httpsClient;
    //Warn Dialog
    LoadingScreen * loadScreen;
    void loadWaitScreen(bool run){
        if (run){
            loadScreen->show();
            loadScreen->startLoading();
            timer->start(TIMEOUT);
        }else{
            if (timer->isActive())
                timer->stop();
            loadScreen->hide();
            loadScreen->stopLoading();
        }
    }
    //timer
    QTimer *timer;
    //setter, changing to struct
    void setUsername(QString user){
        this->username = user;
    }
    const QString &getUsername(){
        return this->username;
    }
    void setToken(const QString &token){
        this->token = token;
    }
    const QString &getToken(){
        return this->token;
    }

signals:

public slots:
    void login(QString username, QString pass);
    void successfulLogin(const QString &username, const QString &token);
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
