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

    if(!this->listen(QHostAddress::Any,port))
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
    //qDeleteAll(connectedUser.begin(), connectedUser.end());
    //clear QVector
    connectedUser.clear();
    //release memory from QVector as no longer clear as of Qt5.7
    connectedUser.squeeze();
    //close this server
    this->close();
}

QByteArray Server::convertedData(const QByteArray &data){
    if (data.size() <= 1)
        return QByteArray();
    QByteArray sendData;
    // raw numbers will be converted to enum
    switch ((int)data[0]){
    case 0: //connected
        sendData.append(data.mid(1));
        sendData.append(QString(" connected"));
        break;
    case 1: //disconnected
        sendData.append(data.mid(1));
        sendData.append(QString(" disconnected"));
        break;
    case 2: //send msg
        sendData.append(data.mid(1));
        sendData.append(QString(" disconnected"));
        break;
    default: //send whole chunk
        return data;
    }
    return sendData;
}

// Slots
void Server::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    //check authentications, decrypt encrypt stuff

    //add into whitelist, generate nonce, store IP, username and userID
    QTcpSocket * client = new QTcpSocket();
    client->setSocketDescriptor(socketDescriptor);

    whiteListObj * whitelistStruct = new whiteListObj();
    //whitelistStruct = (whiteListObj) {"127.0.0.1", "Something", 1, client, 1};
    whitelistStruct->IPaddress = "127.0.0.1";
    whitelistStruct->userName = "something";
    whitelistStruct->userId = 1;
    whitelistStruct->socket = client;
    whitelistStruct->nonce = 1;
    connectedUser.append(whitelistStruct);

    QObject::connect(client, &QTcpSocket::readyRead, [this,client]()
    {
        QByteArray data = client->readAll();
        //decrypt data at here, send unencrypted form, check data
        sendToAll(data);
    });

}

void Server::sendToAll(QByteArray data)
{
    int i = 0;
    // verify data send
    if (data.size() <= 1)
        return;

    //may move to separate function
    while (1){
        if (i >= connectedUser.size())
            break;
        if (connectedUser[i]->socket->state() == QAbstractSocket::ConnectedState){
            connectedUser[i]->socket->write(data);
            ++i;
        }else{
            connectedUser.remove(i);
        }
    }
    qDebug() << "Number of connected users " << connectedUser.size();
}
