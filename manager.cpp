#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    //UI init
    startWindowUi = new MainWindow();
    currentUi = startWindowUi;
    startWindowUi->show();
    createAccUi = NULL;
    chatGrpUi = NULL;
    chatRmUi = NULL;
    createGrpUi = NULL;

    //user data init
    username = tr("");
    token = tr("");
    socServer = NULL;
    socClient = NULL;

    //warning dialog
    loadScreen = new LoadingScreen();

    //timer
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [this](){
       loadWaitScreen(false);
    });

    //startwindow connect
    connect(startWindowUi, SIGNAL(signIn(QString, QString)),
            this, SLOT(login(QString, QString)));
    connect(startWindowUi, SIGNAL(createAcc()), this, SLOT(registerAcc()));
    //https client, communication to web server
    connect(&httpsClient, SIGNAL(returnLoginToken(const QString &, const QString &)), this, SLOT(successfulLogin(const QString &, const QString &)));
    connect(&httpsClient, &SSLClient::errorOccur, [this](QString msg){ displayMessageBox(msg); });
    connect(&httpsClient, &SSLClient::returnChatRoomToken,
        [this](const QString &chatsession, const QString &iv, const QString &encAuth){
            if (this->socClient) { this->socClient->sendServerVerify(chatsession, iv, encAuth); }
    });
    connect(&httpsClient, &SSLClient::logoutSuccess, [this](){
        this->username = "";
        this->token = "";
    });
}

Manager::~Manager()
{
    delete startWindowUi;
    // auto delete rest of Ui;
}

void Manager::displayMessageBox(QString msg){
    QMessageBox::warning(currentUi, "System Message", msg,QMessageBox::Ok);

}

// Slots
void Manager::login(QString username, QString pass)
{
    /*
     * This function attempt to authenicate with web server
     *
     * Signal from: startWindowUi::signIn
     * *Assumed username and password was validated at startWindowUi before passing
     * Input: QString username and QString password
     * Output:
     * 1)   SSLClient::loginToken -> SSLClient::recieveLoginTokenSuccessful -> Manager::successfulLogin
     *      Will show Manager::successfulLogin as successful login
     * 2)   SSLClient::loginToken -> SSLClient::recieveLoginTokenSuccessful -> Manager::displayMessageBox
     *      Will show Manager::displayMessageBox displaying related error msg
     */
    // User validation
    httpsClient.loginToken(username, pass);

    //wait screen
    loadWaitScreen(true);
}

void Manager::successfulLogin(const QString &username,const QString &token)
{
    /*
     * This function recieved and save authenticated username and current session token
     *
     * Signal from: SSLClient::recieveLoginTokenSuccessful
     * Input: QString username and QString token
     * Output:
     *     UI transistion = startWindowUi to chatGrpUi
     */
    //set variables
    setUsername(username);
    setToken(token);

    //hide loading
    loadWaitScreen(false);

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
    //set currentUi
    currentUi = chatGrpUi;
    chatGrpUi->show();
}

void Manager::registerAcc()
{
    if(!createAccUi){
        //createAccUi = new CreateAccount(this);
        createAccUi = new CreateAccount(startWindowUi);
        connect(createAccUi,SIGNAL(goMain()), this, SLOT(showMain()));
        connect(createAccUi, &CreateAccount::registerAcc, [this](QString username, QString password){
            //send to server to process
            httpsClient.registerAcc(username, password);
        });
        createAccUi->setModal(true);
    }
    currentUi = createAccUi;
    createAccUi->show();
}

void Manager::showMain()
{
    // clear username, user data
    setUsername("");

    currentUi = startWindowUi;
    startWindowUi->show();
}

void Manager::showChatGroup()
{
    currentUi = chatGrpUi;
    chatGrpUi->show();
    //close server if on;
    if (socServer){
        socServer->closeServer();
        delete socServer;
        socServer = NULL;
        qDebug() << "Server closed";
    }
    //close client socket if on
    if (socClient){
        socClient->shut();
        qDebug() << "Client related closed";
        //delete socClient;
    }
}

void Manager::showChatRoom()
{
    //check if exist
    if (!socClient){
        socClient = new ChatClient(this);
        //connect socClient
        connect(socClient, SIGNAL(displayMsg(QString,QString)), this, SLOT(displayMsg(QString,QString)));
        connect(socClient, SIGNAL(verifyToken(QString)), this, SLOT(verifyChatRoomToken(QString)));
        connect(socClient, SIGNAL(disconnected()), this, SLOT(chatRoomDisconnected()));
        connect(socClient, SIGNAL(roomFull()), this, SLOT(chatRoomFull()));
        connect(socClient, &ChatClient::userJoin, [this](QString user){
            chatRmUi->addUser(user);
        });
        connect(socClient, &ChatClient::userQuit, [this](QString user){
            chatRmUi->removeUser(user);
        });
    }
    //set username
    socClient->setUsername(this->username);

    // if own hosting
    if (socServer){
        socClient->connectToHost(QHostAddress(QHostAddress::LocalHost).toString());
    }else{
        // get ip from somewhere
        socClient->connectToHost("192.168.1.134");
        //update if fail to connect
    }
    // delete previous chatroom
    if (chatRmUi)
        delete chatRmUi;

    chatRmUi = new Chatroom(startWindowUi, username);
    //connect chat room
    connect(chatRmUi, SIGNAL(leaveRoom()), this, SLOT(showChatGroup()));
    connect(chatRmUi, SIGNAL(typeMsg(QString)), this, SLOT(chatRoomSendMsg(QString)));
    //show chat room
    chatRmUi->setModal(true);
    currentUi = chatRmUi;
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
    currentUi = createGrpUi;
    createGrpUi->show();
}

void Manager::hostServer()
{
    //Run server
    qDebug() << "Running server at port 1234";

    socServer = new Server(this);
    socServer->startServer();

    //show chatroom
    showChatRoom();
}

void Manager::displayMsg(QString sender, QString msg){
    chatRmUi->displayMsg(sender, msg);
}

void Manager::chatRoomConnected()
{
    //clientSocket->write( QByteArray("0") + qPrintable(getUsername()) );
}

void Manager::chatRoomDisconnected()
{
    qDebug() << "Disconnected";
    // show chat group
    showChatGroup();
    // show message box
    displayMessageBox("Host has quit the chat room.");
}

void Manager::chatRoomSendMsg(QString msg)
{
    if (socClient){
        socClient->sendMsg(msg);
    }
}

void Manager::chatRoomFull(){
    qDebug() << "Room full";
    // show chat group
    showChatGroup();
    // show message box
    displayMessageBox("Chat room is full.");
}

void Manager::verifyChatRoomToken(QString token){
    qDebug() << "Recieved " << token;
    httpsClient.chatRoomToken(this->username, this->token, token);
}

