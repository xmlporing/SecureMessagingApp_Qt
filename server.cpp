#include "server.h"

// Constructor & Destructor
Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    connectedUser = {};
}

Server::~Server()
{
    this->close();
    qDebug() << "Closing server";
}

// Function
void Server::startServer()
{
    int port = 1234;

    if(!this->listen(QHostAddress::LocalHost,port))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << port << "...";
    }
}

void Server::closeServer()
{
    delete this;
}

// Slots
void Server::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    // dynamically created, haven't deleted
    //QTcpSocket client = *(this->nextPendingConnection());
    //connectedUser.append(*this->nextPendingConnection());
    //QTcpSocket client = this->nextPendingConnection();
    connectedUser.append(*this->nextPendingConnection());
    QTcpSocket * client = &connectedUser.back();
    /*
    if(!client->setSocketDescriptor(socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(client->error());
        return;
    }
    */
    //connect(client, &QTcpSocket::readyRead, this, SLOT(sendToAll()));
    QObject::connect(client, &QTcpSocket::readyRead, [this,client]()
    {
        //qDebug() << client->readAll();
        QByteArray data = client->readAll();
        //QString qmessage(Data);
        sendToAll(data);
    });
}

void Server::updateChatroom(QString text)
{
    //not used
    emit updateUI(text);
}

void Server::sendToAll(QByteArray data)
{
    for (connectedUserIter = connectedUser.begin(); connectedUserIter != connectedUser.end(); connectedUserIter++)
    {
        connectedUserIter->write(data);
    }
}
