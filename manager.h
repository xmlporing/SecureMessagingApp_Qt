#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
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
    //UI / pages
    MainWindow *startWindowUi;
    CreateAccount *createAccUi;
    Chatgroup *chatGrpUi;
    Chatroom *chatRmUi;
    CreateGroup *createGrpUi;
    //track ui
    QWidget* currentUi;
    void trackUi(QWidget* newUi);
    //user data
    QString username;
    QString token;
    QString groupName;
    //chat server
    Server *socServer;
    ChatClient *socClient;
    //HTTPS client
    SSLClient httpsClient;
    //warning dialog
    LoadingScreen * loadScreen;
    void loadWaitScreen(bool run);
    //timer
    QTimer *timer;
    //setter
    void setUsername(QString user);
    void setToken(QString token);
    void setGroupName(QString groupName);
    //getter
    const QString &getUsername();
    const QString &getToken();
    const QString &getGroupName();
    //message box
    void displayMessageBox(QString msg);

signals:

public slots:
    void login(QString username, QString pass);
    void successfulLogin(const QString &username, const QString &token);
    void registerAcc();
    void showChatGroup();
    void connectToChatRoom(QString ip);
    void showChatRoom();
    void showCreateGroup();
    void hostServer(int groupSize);
    void displayMsg(QString user, QString msg);
};

#endif // MANAGER_H
