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
    void init();
    //setter
    void setUsername(QString username);
    void setGroupName(QString groupName);
    //getter
    const bool& getConnectivity();
    const QString getUsername(int userid);
    //connection
    bool connectToHost(QString ip);
    void shut();
    //packets
    void rejectPacket(EType e);
    void sendPacket(TType t,QString contents);
    void sendMsg(QString msg);

private:
    QTcpSocket * clientSoc;
    int nonce;
    int ownID;
    QString ownUsername;
    QString ownGroupName;
    QVector<UserList> userList;
    bool connectionState;
    bool checkDisconnect;
    CryptoPP::SecByteBlock key;
    QByteArray prevData;

signals:
    void displayMsg(QString sender, QString msg);
    void verifyToken(QString token);
    void error(QString errormsg);
    void userJoin(QString username);
    void userQuit(QString username);

public slots:
    void readPacket();
    void initConnection();
    void sendServerVerify(QString sessionkey,QString verifyIV, QString encAuth);
};

#endif // CHATCLIENT_H
