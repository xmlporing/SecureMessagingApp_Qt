#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent)
{
    //UI  / pages init
    startWindowUi = new MainWindow();
    createAccUi = NULL;
    chatGrpUi = NULL;
    chatRmUi = NULL;
    createGrpUi = NULL;

    //track ui init
    currentUi = NULL;
    trackUi(startWindowUi);

    //user data init
    username = tr("");
    token = tr("");

    //chat server init
    socServer = NULL;
    socClient = NULL;

    //warning dialog init
    loadScreen = new LoadingScreen();

    //timer init
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [this](){
       loadWaitScreen(false);
    });

    //startwindow connect
    connect(startWindowUi, SIGNAL(signIn(QString, QString)),
            this, SLOT(login(QString, QString)));
    connect(startWindowUi, SIGNAL(createAcc()), this, SLOT(registerAcc()));
    connect(startWindowUi, &MainWindow::errorOccur, [this](QString errMsg){
       this->displayMessageBox(errMsg);
    });

    //https client, communication to web server
    // successful login
    connect(&httpsClient, &SSLClient::returnLoginToken,[this](const QString& username, const QString& token){
        this->successfulLogin(username, token);
    });
    // successful logout
    connect(&httpsClient, &SSLClient::logoutSuccess, [this](){
        //clear all details
        this->setUsername("");
        this->setToken("");
        this->setGroupName("");
        //go to main
        this->trackUi(this->startWindowUi);
    });
    // chat group entry recieved
    connect(&httpsClient, &SSLClient::newChatGroupList, [this]
            (QString groupName, int groupSize,int groupCount, QString ip){
        //update chat group list
        this->chatGrpUi->addChatGroup(groupName, groupSize, groupCount, ip);
    });
    // successful chat room created
    connect(&httpsClient, &SSLClient::createChatRoomSuccess, [this](){
        //socServer will be destroyed if unable to host
        if (this->socServer){
            //ip will be converted to local
            this->connectToChatRoom("");
        }
    });
    // successful chat room token recieved
    connect(&httpsClient, &SSLClient::returnJoinChatRoomToken, [this]
            (QString chatsession,QString iv, QString encAuth){
        //send to verify with chat server
        if(this->socClient){
            this->socClient->sendServerVerify(chatsession, iv, encAuth);
        }
    });
    // successful chat room count update
    connect(&httpsClient, &SSLClient::updateChatRoomSuccess, [this](){
        // success, dont need update UI
    });
    // successful chat room deletion
    connect(&httpsClient, &SSLClient::delChatRoomSuccess, [this](){
        // success, dont need update UI
    });
    // recieve error, display it
    connect(&httpsClient, &SSLClient::errorOccur, [this](QString msg){
        displayMessageBox(msg);
    });
    // send success message
    connect(&httpsClient, &SSLClient::successMsg, [this](QString msg){
        trackUi(startWindowUi);
        displayMessageBox(msg);
    });
}

Manager::~Manager()
{
    delete startWindowUi;
    // auto delete rest of Ui;
}

// track ui
void Manager::trackUi(QWidget* newUi){
    /*
     * This function display error to user
     *
     * Signal/Call from: Any
     * Input: New UI that suppose to show
     * Output: Hide previous UI, show new UI
     */
    if(currentUi)
        currentUi->hide();
    currentUi = newUi;
    currentUi->show();
}

// warning dialog
void Manager::loadWaitScreen(bool run){
    if (run){
        loadScreen->show();
        loadScreen->startLoading();
        timer->start(LTIMEOUT);
    }else{
        if (timer->isActive())
            timer->stop();
        loadScreen->hide();
        loadScreen->stopLoading();
    }
}

// Setter
void Manager::setUsername(QString user){
    this->username = user;
}
void Manager::setToken(QString token){
    this->token = token;
}
void Manager::setGroupName(QString groupName){
    this->groupName = groupName;
}

// Getter
const QString& Manager::getUsername(){
    return this->username;
}
const QString& Manager::getToken(){
    return this->token;
}
const QString& Manager::getGroupName(){
    return this->groupName;
}

// Message box
void Manager::displayMessageBox(QString msg){
    /*
     * This function display error to user
     *
     * Signal from: Any
     * Input: Error message
     * Output: QMessageBox with only "OK" option
     */
    //close loadWaitScreen
    this->loadWaitScreen(false);
    QMessageBox::warning(currentUi, "System Message", msg,QMessageBox::Ok);
}

//

// ******* Slots *******
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
     *     create chatGrpUi if has not been created
     */
    //set variables
    setUsername(username);
    setToken(token);

    //hide loading
    loadWaitScreen(false);

    //clear away user details
    startWindowUi->clearUserDetails();

    //check if chat group created before
    if(!chatGrpUi){
        qDebug() << "create chat group";
        chatGrpUi = new Chatgroup(startWindowUi);
        //connect signal
        // logout
        connect(chatGrpUi, &Chatgroup::goMain, [this](){
            //attempt to logout
            this->httpsClient.logoutAcc(this->getUsername(),this->getToken());
        });
        // join chat room
        connect(chatGrpUi, &Chatgroup::joinRoom, [this](QString grpName, QString ip){
            //set joined chat room name
            this->setGroupName(grpName);
            //connect to chat room
            this->connectToChatRoom(ip);
        });
        // show create chat room UI
        connect(chatGrpUi, &Chatgroup::makeGroup, [this](){
            //show create chat room UI
            this->showCreateGroup();
        });
        // refresh list
        connect(chatGrpUi, &Chatgroup::updateGroupList, [this](){
            //https get list
            this->httpsClient.getChatRoom(this->getUsername(),this->getToken());
        });
        // error
        connect(chatGrpUi, &Chatgroup::errorOccur, [this](QString errMsg){
            //display error
            this->displayMessageBox(errMsg);
        });
        chatGrpUi->setModal(true);
    }
    //refresh list
    chatGrpUi->refreshList();
    //set currentUi
    trackUi(chatGrpUi);
}

void Manager::registerAcc()
{
    /*
     * This function show registration UI
     *
     * Signal from: startWindowUi::createAcc
     * Input: Nil
     * Output: Show registration UI and create it if has not been created
     */
    //delete previous, so all details are deleted
    if(createAccUi){
        delete createAccUi;
        createAccUi = NULL;
    }
    //create new
    createAccUi = new CreateAccount(startWindowUi);
    //connect signal
    // register account
    connect(createAccUi, &CreateAccount::registerAcc, [this](QString username, QString password){
        //send to server to process
        httpsClient.registerAcc(username, password);
    });
    // error
    connect(createAccUi, &CreateAccount::errorOccur, [this](QString errMsg){
        //display error msg
        this->displayMessageBox(errMsg);
    });
    createAccUi->setModal(true);
    //show UI
    createAccUi->show();
}

void Manager::showChatGroup()
{
    /*
     * This function show chatGrpUi after closing existing chat server/client
     *
     * Called from: Chatroom::leaveRoom, CreateGroup::cancelCreate,
     * Input: QString username and QString token
     * Output: Show chat group UI
     */
    //close server if on;
    if (socServer){
        //delete server
        delete socServer;
        socServer = NULL;
        //call to close group chat
        httpsClient.delChatRoom(this->getUsername(),this->getToken());
        qDebug() << "Server closed";
    }
    //reset groupName
    setGroupName("");
    //set currentUi
    trackUi(chatGrpUi);
}

void Manager::connectToChatRoom(QString ip)
{
    /*
     * This function attempt to connect ip's chat server
     * while creating a new chat client if it is not created
     *
     * Input: QString ip for client to connect
     * Output: Show chat group UI
     */
    //check if exist
    if (!socClient){
        socClient = new ChatClient(this);
        //connect signal
        // displayMsg that chat client recieve
        connect(socClient, SIGNAL(displayMsg(QString,QString)), this, SLOT(displayMsg(QString,QString)));
        // send token to web server to verify
        connect(socClient, &ChatClient::verifyToken, [this](QString token){
            qDebug() << "Recieved " << token;
            //web server to authenticate
            this->httpsClient.joinChatRoom(this->getUsername(), this->getToken(),
                                     this->getGroupName(), token);
        });
        // error
        connect(socClient, &ChatClient::error, [this](QString errMsg){
            //disconnect socket
            socClient->shut();
            //quit chat room
            this->showChatGroup();
            //display error
            this->displayMessageBox(errMsg);
            //debug
            qDebug() << "Error recieve";
        });
        // new user connected
        connect(socClient, &ChatClient::userJoin, [this](QString user){
            this->chatRmUi->addUser(user);
        });
        // connected user quit
        connect(socClient, &ChatClient::userQuit, [this](QString user){
            this->chatRmUi->removeUser(user);
        });
    }
    //set username
    socClient->setUsername(this->username);

    QString IP = ip;
    // if own hosting
    if (socServer){
        IP = QHostAddress(QHostAddress::LocalHost).toString();
    }
    //only if success than show chat room
    if (socClient->connectToHost(IP)){
        //enter chatroom
        this->showChatRoom();
    }else{
        //init chat client shutdown
        socClient->shut();
        //display error
        this->displayMessageBox("Error connecting");
    }
}

void Manager::showChatRoom()
{
    /*
     * This function show chat room UI
     *
     * Input: Nil
     * Output: Show new chat room UI
     */
    //delete previous chatroom
    if (chatRmUi)
        delete chatRmUi;
    //make new chatRmUi
    chatRmUi = new Chatroom(startWindowUi, username);
    //connect chat room
    connect(chatRmUi, &Chatroom::leaveRoom, [this](){
       //shut socket
       socClient->shut();
    });
    // sending msg to host
    connect(chatRmUi, &Chatroom::typeMsg, [this](QString msg){
        if (this->socClient){
            this->socClient->sendMsg(msg);
        }
    });
    // invalid user inputs
    connect(chatRmUi, &Chatroom::errorOccur, [this](QString errMsg){
       this->displayMessageBox(errMsg);
    });
    //show chat room
    chatRmUi->setModal(true);

    //set currentUi
    trackUi(chatRmUi);
}

void Manager::showCreateGroup()
{
    /*
     * This function show create chat group UI
     *
     * Input: Nil
     * Output: Show create chat group UI
     */
    if(!createGrpUi){
        //new ui
        createGrpUi = new CreateGroup(startWindowUi);
        //connect signal
        // leave creating chat group
        connect(createGrpUi, SIGNAL(cancelCreate()), this, SLOT(showChatGroup()));
        // attempt to create chat group
        connect(createGrpUi, &CreateGroup::createdGroup, [this]
                (QString groupName, int groupSize, int groupCount, QString ip){
            //host chat server
            this->hostServer(groupSize);
            //update web server
            this->httpsClient.createChatRoom(this->getUsername(), this->getToken(),
                                             groupName, groupSize,
                                             groupCount, ip);
        });
        // error
        connect(createGrpUi, &CreateGroup::errorOccur, [this](QString errMsg){
            //display error
            this->displayMessageBox(errMsg);
        });
        createGrpUi->setModal(true);
    }
    //set currentUi
    trackUi(createGrpUi);
}

void Manager::hostServer(int groupSize)
{
    /*
     * This function attempt to host chat server with specified group size
     *
     * Input: int groupSize for limiting number of connected user
     * Output: Server start listening at PORT
     */
    //Run server
    qDebug() << "Running server at port " << PORT;
    socServer = new Server(this);
    //check if unable to start
    if(socServer->startServer(groupSize)){
        qDebug() << "No issue";
        //set masterkey for token verification
        socServer->setMasterKey(this->token);
        //set host username
        socServer->setUsername(this->getUsername());
        qDebug() << "Connecting signal";
        //connect signal
        // update web server count
        connect(socServer, &Server::updateCount, [this](int count){
           //web server commmunication
           this->httpsClient.updateChatRoom(this->getUsername(),
                                            this->getToken(),QString::number(count));
        });
        // error
        connect(socServer, &Server::error, [this](QString errMsg){
           //display error
           this->displayMessageBox(errMsg);
        });
        // quit chat room
        connect(socServer, &Server::delGrp, [this](){
           //update server
            this->httpsClient.delChatRoom(this->getUsername(),this->getToken());
        });
        qDebug() << "socServer setup hao le";
    }else{
        qDebug() << "Deleting";
        //delete chat server
        delete socServer;
        socServer = NULL;
        qDebug() << "deleted";
    }
}

void Manager::displayMsg(QString sender, QString msg){
    chatRmUi->displayMsg(sender, msg);
}
