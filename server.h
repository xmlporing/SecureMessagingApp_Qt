#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QString>
#include <QVector>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QDataStream>
#include "network.h"
#include "library.h"

class Server : public QTcpServer
{
    Q_OBJECT

    struct whiteListObj{
        QString IPaddress;          //IP address of user
        QString userName;           //username of user
        int userId;                 //chat server allocated id
        QTcpSocket * socket;        //connection to user
        QString token;              //verification token
        int nonce;                  //nonce for host quit
        CryptoPP::SecByteBlock key; //session key between user
        bool verified;              //flag for verification
        whiteListObj(){}
        whiteListObj(QString ip, int userid, QTcpSocket* client, bool verify){
            IPaddress = ip;
            userId = userid;
            socket = client;
            verified = verify;
            nonce = 0;
            userName = token = "";
            key = CryptoPP::SecByteBlock(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
        }

        ~whiteListObj(){
            socket->abort();
            socket->deleteLater();
            socket = NULL;
        }
    };
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    //connection
    bool startServer(int maxUserCount);
    void closeServer();
    //packet
    void rejectPacket(const whiteListObj* wlObj, EType error);
    void sendPacket(whiteListObj* wlObj, TType t, QString contents);
    //key
    void setMasterKey(QString key);
    //set host username
    void setUsername(QString username);

private:
    QVector<whiteListObj*> connectedUser;
    int currentId;
    int maxCount;
    CryptoPP::SecByteBlock masterKey;
    QString ownUsername;

    void processIncomingData(QByteArray data, whiteListObj* wlObj);
    bool compareIP(const QHostAddress & ip);
    QString generateToken();
    int generateNonce();

signals:
    void updateCount(int count);
    void error(QString errormsg);
    void delGrp();

public slots:
    void sendToAll(QString data, TType t = PROTOCOL_TYPE::Message);

protected:
    //base function
    void incomingConnection(qintptr socketDescriptor);    

};

#endif // SERVER_H
