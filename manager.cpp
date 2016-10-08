#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    //UI init
    startWindowUi = new MainWindow();
    startWindowUi->show();
    createAccUi = NULL;
    chatGrpUi = NULL;
    chatRmUi = NULL;
    createGrpUi = NULL;

    //user data init
    username = tr("");
    socServer = NULL;

    //connect
    connect(startWindowUi, SIGNAL(signIn(QString, QString)),
            this, SLOT(login(QString, QString)));
    connect(startWindowUi, SIGNAL(createAcc()), this, SLOT(registerAcc()));
}

Manager::~Manager()
{
    delete startWindowUi;
    //delete createAccUi;
    //delete chatGrpUi;
    //delete chatRmUi;
    //delete createGrpUi;
}

// Setters
void Manager::setUsername(QString user)
{
    username = user;
}

// Getters
const QString &Manager::getUsername()
{
    return username;
}

// Slots
void Manager::login(QString username, QString pass)
{
    // can do all the validation here
    // if username and password is success
    if (username != "user" && pass != "password")
    {
        QMessageBox msgBox;
        msgBox.setText("Your login credential is incorrect, please try again or register.");
        msgBox.exec();
    }
    else
    {
        setUsername(username);
        //hide main
        startWindowUi->hide();

        //check if chat group created before
        if(!chatGrpUi){
            qDebug() << "create chat group";
            chatGrpUi = new Chatgroup(startWindowUi);
            connect(chatGrpUi, SIGNAL(goMain()), this, SLOT(showMain())); //connect
            connect(chatGrpUi, SIGNAL(joinRoom()), this, SLOT(showChatRoom()));
            connect(chatGrpUi, SIGNAL(makeGroup()), this, SLOT(showCreateGroup()));
            chatGrpUi->setModal(true);
        }
        chatGrpUi->show();
    }
}

void Manager::registerAcc()
{
    if(!createAccUi){
        //createAccUi = new CreateAccount(this);
        createAccUi = new CreateAccount(startWindowUi);
        connect(createAccUi,SIGNAL(goMain()), this, SLOT(showMain()));
        createAccUi->setModal(true);
    }
    createAccUi->show();
}

void Manager::showMain()
{
    // clear username, user data
    setUsername("");

    startWindowUi->show();
}

void Manager::showChatGroup()
{
    //close server if on;
    if (socServer){
        socServer->closeServer();
        socServer = NULL;
    }
    chatGrpUi->show();
}

void Manager::showChatRoom()
{
    /*
    if(!chatRmUi){
        chatRmUi = new Chatroom(this, username);
        connect(chatRmUi, SIGNAL(leaveRoom()), this, SLOT(showChatGroup()));
        chatRmUi->setModal(true);
        //connect(chatRmUi, SIGNAL(leaveRoom()), chatGrpUi, SLOT(exec()));
    }
    */
    //chatRmUi = new Chatroom(this, username);
    clientSocket = new QTcpSocket(this);
    connect(clientSocket, SIGNAL(connected()), this, SLOT(chatRoomConnected()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(chatRoomDisconnected()));
    //connect(clientSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(chatRoomReadyRead()));

    clientSocket->connectToHost("127.0.0.1", 1234);

    if(!clientSocket->waitForConnected(5000))
    {
        qDebug() << "Error: " << clientSocket->errorString();
    }

    chatRmUi = new Chatroom(startWindowUi, username);
    connect(chatRmUi, SIGNAL(leaveRoom()), this, SLOT(showChatGroup()));
    connect(chatRmUi, SIGNAL(typeMsg(QString)), this, SLOT(chatRoomSendMsg(QString)));
    chatRmUi->setModal(true);
    chatRmUi->show();
}

void Manager::showCreateGroup()
{
    if(!createGrpUi){
        //createGrpUi = new CreateGroup(this);
        createGrpUi = new CreateGroup(startWindowUi);
        connect(createGrpUi, SIGNAL(cancelCreate()), this, SLOT(showChatGroup()));
        connect(createGrpUi, SIGNAL(createdGroup()), this, SLOT(hostServer()));
        createGrpUi->setModal(true);
    }
    createGrpUi->show();
}

void Manager::hostServer()
{
    /*
    //Run server
    qDebug() << "Running server at port 1234";

    socServer = new Server(this);
    // connect signal/slot
    connect(socServer, SIGNAL(updateUI(QString)), this, SLOT(displayMsg(QString)));

    //Start thread
    //mThread->start();
    socServer->startServer();
    */
    //show chatroom
    showChatRoom();
}

void Manager::displayMsg(QString msg){
    chatRmUi->displayMsg(msg);
}

void Manager::chatRoomConnected()
{
    clientSocket->write( qPrintable(getUsername() + tr(" connected.")) );
}

void Manager::chatRoomDisconnected()
{
    qDebug() << "Disconnected";
}

void Manager::chatRoomReadyRead()
{
    QByteArray data = clientSocket->readAll();
    std::string message(data.constData(), data.length());
    QString messageRecieved = QString::fromStdString(message);
    displayMsg(messageRecieved);
}

void Manager::chatRoomSendMsg(QString msg)
{
    clientSocket->write( qPrintable(msg) );
}
