#ifndef SERVER_H
#define SERVER_H


#include <QTcpServer>
#include <QString>
#include "serverthread.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    void startServer();
    void closeServer();

signals:
    void updateUI(QString);

public slots:
    void updateChatroom(QString);

protected:
    void incomingConnection(qintptr socketDescriptor);

};

#endif // SERVER_H
