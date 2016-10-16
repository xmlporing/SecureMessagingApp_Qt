#include "server.h"

// Constructor & Destructor
Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    connectedUser = {};
}

Server::~Server()
{
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
    //delete all connected qtcpsocket
    qDeleteAll(connectedUser.begin(), connectedUser.end());
    //clear QVector
    connectedUser.clear();
    //release memory from QVector as no longer clear as of Qt5.7
    connectedUser.squeeze();
    //close this server
    this->close();
}

// Slots
void Server::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";


    QTcpSocket * client = new QTcpSocket();
    client->setSocketDescriptor(socketDescriptor);
    connectedUser.append(client);

    QObject::connect(client, &QTcpSocket::readyRead, [this,client]()
    {
        //qDebug() << client->readAll();
        QByteArray data = client->readAll();
        //QString qmessage(Data);
        sendToAll(data);
    });

}

void Server::sendToAll(QByteArray data)
{
    int i = 0;
    //may move to separate function
    while (1){
        if (i >= connectedUser.size())
            break;
        if (connectedUser[i]->state() == QAbstractSocket::ConnectedState){
            connectedUser[i]->write(data);
            ++i;
        }else{
            connectedUser.remove(i);
        }
    }
    qDebug() << "Number of connected users " << connectedUser.size();
}
