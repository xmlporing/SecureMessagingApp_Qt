#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QString>
#include <QVector>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QDataStream>
#include <QMutex>
#include <QMutexLocker>
#include "network.h"

class Server : public QTcpServer
{
    Q_OBJECT

    struct whiteListObj{
        QString IPaddress;
        QString userName;
        int userId;
        QTcpSocket * socket;
        QString token;
        int nonce;
        CryptoPP::SecByteBlock key;
        bool verified;
        whiteListObj(){}
        whiteListObj(QString ip, int userid, QTcpSocket* client, bool verify){
            IPaddress = ip;
            userId = userid;
            socket = client;
            verified = verify;
            nonce = 0; //
            userName = token = "";
            key = CryptoPP::SecByteBlock(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
        }

        ~whiteListObj(){
            if (socket)
                delete socket;
        }
    };
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    void startServer();
    void closeServer();
    void rejectPacket(const whiteListObj* wlObj, EType error);
    void sendPacket(const whiteListObj* wlObj, TType t, QString contents);

private:
    QVector<whiteListObj*> connectedUser;
    int currentId;
    CryptoPP::SecByteBlock masterKey;
    void processIncomingData(QByteArray data, whiteListObj* wlObj);
    QByteArray convertedData(const QByteArray &); //***
    bool compareIP(const QHostAddress & ip);
    QString generateToken();
    int generateNonce();
    QMutex mutex;

signals:
    //void error(QTcpSocket::SocketError socketerror);
    void updateCount(int count);

public slots:
    void sendToAll(QString data, TType t = PROTOCOL_TYPE::Message);

protected:
    void incomingConnection(qintptr socketDescriptor);    

};

#endif // SERVER_H
