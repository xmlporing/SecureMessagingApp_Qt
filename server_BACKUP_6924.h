#ifndef SERVER_H
#define SERVER_H


#include <QTcpServer>
#include <QString>
#include <QVector>
#include <QTcpSocket>

struct whitelistObj{

};

class Server : public QTcpServer
{
    Q_OBJECT

    struct whiteListObj{
        QString IPaddress;
        QString userName;
        qint32 userId;
        QTcpSocket * socket;
        qint32 nonce;
    };
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    void startServer();
    void closeServer();

private:
<<<<<<< HEAD
    QVector<QTcpSocket *> connectedUser;
    QByteArray convertedData(const QByteArray &);
=======
    QVector<whiteListObj *> connectedUser;
>>>>>>> 5a7d41f20ebba0ea546e8a866e76f168ef5b199b

signals:
    //void error(QTcpSocket::SocketError socketerror);

public slots:
    void sendToAll(QByteArray);

protected:
    void incomingConnection(qintptr socketDescriptor);    

};

#endif // SERVER_H
