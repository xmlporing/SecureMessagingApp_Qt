#ifndef SERVER_H
#define SERVER_H


#include <QTcpServer>
#include <QString>
#include <QVector>
#include <QTcpSocket>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    void startServer();
    void closeServer();

private:
    QVector<QTcpSocket *> connectedUser;

signals:
    //void error(QTcpSocket::SocketError socketerror);

public slots:
    void updateChatroom(QString);
    void sendToAll(QByteArray);

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // SERVER_H
