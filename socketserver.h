#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QTcpServer>
#include <QString>
#include "serverthread.h"

class SocketServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SocketServer(QObject *parent = 0);
    ~SocketServer();
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor);

private slots:
    void updateChatroom(QString);

signals:
    void displayMessage(QString);

};

#endif // SOCKETSERVER_H
