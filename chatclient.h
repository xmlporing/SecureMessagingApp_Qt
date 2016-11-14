#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QVector>
#include <QDataStream>
#include "network.h"

class ChatClient : public QObject
{
    Q_OBJECT
    struct UserList{
        QString userName;
        int userId;
        UserList(){}
        UserList(int userid, QString username){
            userId = userid;
            userName = username;
        }
        ~UserList(){}
    };
public:
    explicit ChatClient(QObject *parent = 0);
    ~ChatClient();
    void shut();
    void init();
    void setUsername(QString username);
    bool connectToHost(QString ip);
    const bool& getConnectivity();
    const QString getUsername(int userid);
    void rejectPacket(EType e);
    void sendPacket(TType t,QString contents);
    void sendMsg(QString msg);

private:
    QTcpSocket * clientSoc;
    int nonce;
    int ownID;
    QString ownUsername;
    QVector<UserList> userList;
    bool connectionState;
    CryptoPP::SecByteBlock key;

signals:
    void displayMsg(QString sender, QString msg);
    void verifyToken(QString token);
    void disconnected();
    void roomFull();
    void userJoin(QString username);
    void userQuit(QString username);

public slots:
    void readPacket();
    void initConnection();
    void sendServerVerify(QString sessionkey,QString verifyIV, QString encAuth);
};

#endif // CHATCLIENT_H
