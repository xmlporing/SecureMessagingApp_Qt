#include "socketserver.h"

SocketServer::SocketServer(QObject *parent) :
    QTcpServer(parent)
{
}

void SocketServer::startServer()
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

void SocketServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    ServerThread *thread = new ServerThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be deleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(messageRecieved(QString)), this, SLOT(updateChatroom(QString)));
    //emit valueChanged(QString("hello %1").arg(i));

    thread->start();
}

void SocketServer::updateChatroom(QString msg){
    emit displayMessage(msg);
}
