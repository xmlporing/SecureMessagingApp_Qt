#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QMessageBox>
#include "mainwindow.h"
#include "createaccount.h"
#include "chatgroup.h"
#include "chatroom.h"
#include "creategroup.h"
#include "server.h"
#include "sslclient.h"
#include "chatclient.h"

#include <QDebug> //debug purpose

#include "loadingscreen.h" //loading screen

#define LTIMEOUT 5000

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
    QWidget* currentUi;
    //user data
    QString username;
    QString token;
    //server
    Server *socServer;
    ChatClient *socClient;
    //HTTPS client
    SSLClient httpsClient;
    //Warn Dialog
    LoadingScreen * loadScreen;
    void loadWaitScreen(bool run){
        if (run){
            loadScreen->show();
            loadScreen->startLoading();
            timer->start(LTIMEOUT);
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
    //Messagebox
    void displayMessageBox(QString msg);

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
    void displayMsg(QString user, QString msg);
    void chatRoomConnected();
    void chatRoomDisconnected();
    void chatRoomSendMsg(QString msg);
    void chatRoomFull();
    void verifyChatRoomToken(QString token);
};

#endif // MANAGER_H
