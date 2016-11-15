#include "chatclient.h"

ChatClient::ChatClient(QObject *parent) : QObject(parent)
{
    //init
    init();
    this->clientSoc = NULL;
}

ChatClient::~ChatClient(){
}

void ChatClient::init(){
    this->nonce = 0;
    this->ownID = 0;
    this->ownUsername = "";
    this->connectionState = false;
    this->key = CryptoPP::SecByteBlock(0x00,CryptoPP::AES::DEFAULT_KEYLENGTH);
    this->userList.clear();
}

//setter
void ChatClient::setUsername(QString username){
    this->ownUsername = username;
}

void ChatClient::setGroupName(QString groupName){
    this->ownGroupName = groupName;
}

//getter
const bool& ChatClient::getConnectivity(){
    return connectionState;
}

const QString ChatClient::getUsername(int userid){
    // loop vector to get username
    const int size = userList.size();
    for (int i = 0; i < size; i++){
        if (userList[i].userId == userid)
            return userList[i].userName;
    }
    // if failed, return empty string
    return QString();
}

//connection
bool ChatClient::connectToHost(QString ip)
{
    // check for valid socket
    if (!clientSoc){
        this->clientSoc = new QTcpSocket(this);
        //connect for connect, disconnect and readyread
        connect(this->clientSoc, SIGNAL(connected()),this, SLOT(initConnection()));
        connect(this->clientSoc, &QTcpSocket::disconnected, [this](){
            emit error("You have been disconnected");
        });
        connect(this->clientSoc, SIGNAL(readyRead()), this, SLOT(readPacket()));
    }
    // try to connect to host
    clientSoc->connectToHost(ip, PORT);
    // unable to connect after timeout
    if (!clientSoc->waitForConnected(TIMEOUT))
        return false;
    // successfully connected
    return true;
}

void ChatClient::shut(){
    init();
    clientSoc->abort();
    clientSoc->deleteLater(); //auto delete
    clientSoc = NULL;
}

//packet
void ChatClient::rejectPacket(EType error){
    // send PROTOCOL_TYPE::Reject with fixed 1 byte length
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(DATASTREAMVER);

    stream << TType(PROTOCOL_TYPE::Reject)
           << LType(PROTOCOL::ErrorSize)
           << EType(error);
    if (this->clientSoc)
        this->clientSoc->write(data, PROTOCOL::HeaderSize + PROTOCOL::ErrorSize);
}

void ChatClient::sendPacket(TType t, QString contents){
    // send packet base on t
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(DATASTREAMVER);

    // IV
    QByteArray iv;
    QByteArray ciphertext = Custom::encrypt(this->key, iv, contents);

    // size
    unsigned int totalsize = iv.size() + ciphertext.size();

    // forming packet
    stream << TType(t)
           << LType(iv.size() + ciphertext.size());
    stream.writeRawData(iv.data(), iv.size());
    stream.writeRawData(ciphertext.data(), ciphertext.size());

    if (this->clientSoc)
        this->clientSoc->write(data, PROTOCOL::HeaderSize + totalsize);
    //save for resend
    prevData = QByteArray(data);
}

void ChatClient::sendMsg(QString msg){
    // send PROTOCOL_TYPE::Message with msg
    sendPacket(PROTOCOL_TYPE::Message, QString::number(this->ownID) + DELIMITER +msg);
}

// slots
void ChatClient::readPacket()
{
    if (!clientSoc)
        return;
    // read all data in packet
    QByteArray data = clientSoc->readAll();
    // check if data is smaller than expected
    if (data.size() < PROTOCOL::HeaderSize)
        return;

    QDataStream dataStream(data);
    dataStream.setVersion(QDataStream::Qt_5_7);

    // check for data
    TType dType = 0x0000;// = static_cast<qint16>data.mid(PROTOCOL::Type,PROTOCOL::TypeSize);
    dataStream >>dType;

    // check for length and length of value is valid
    LType dLength = 0x00000000;// = static_cast<qint32>(data.mid(PROTOCOL::Length,PROTOCOL::LengthSize));
    dataStream >> dLength;
    if (data.size() != static_cast<int>(dLength) + PROTOCOL::HeaderSize){
        qDebug() << "Different in length, packet size: "
                 << data.size()
                 << " , length value: "
                 << dLength
                 << " , Client";
        return;
    }
    switch (dType){
    case PROTOCOL_TYPE::TokenVerify:
    {
        //send token to https
        QByteArray token;
        //resize
        token.resize((int)dLength);
        dataStream.readRawData(token.data(), (int)dLength);
        //send to manager
        emit verifyToken(QString::fromLocal8Bit(token));
        break;
    }
    case PROTOCOL_TYPE::SendNonce:
    {
        //update nonce
        QByteArray iv, ciphertext;
        //resize
        iv.resize((int)PROTOCOL::IVSize);
        ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
        //add to qbytearry
        dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
        dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
        //decrypt
        QString text = Custom::decrypt(this->key, iv, ciphertext);
        QStringList pieces = text.split(DELIMITER);
        if (pieces.size() == MESSAGE::Section){
            QString nonce = pieces[MESSAGE::UserId];
            //set nonce
            bool convertOk;
            this->nonce = nonce.toInt(&convertOk); //default to 0 if fail
            QString id = pieces[MESSAGE::MsgValue];
            //set id
            bool convertOk2;
            this->ownID = id.toInt(&convertOk2); //default to 0 if fail
            if (!convertOk2 || !convertOk){
                rejectPacket(ERROR::InvalidSessionNonce);
                break;
            }
            //update connectivity
            this->connectionState = true;
            //send username to server
            sendPacket(PROTOCOL_TYPE::UserDetails, this->ownUsername);
        }
        break;
    }
    case PROTOCOL_TYPE::Message:
    {
        //update chat room
        QByteArray iv, ciphertext;
        //resize
        iv.resize((int)PROTOCOL::IVSize);
        ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
        //add to qbytearry
        dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
        dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
        //decrypt
        QString text = Custom::decrypt(this->key, iv, ciphertext);
        //split by DELIMITER
        QStringList pieces = text.split(DELIMITER);
        if (pieces.size() >= MESSAGE::Section){
            QString textUserid = pieces[MESSAGE::UserId];
            //convert to int
            bool convertOk;
            int userid = textUserid.toInt(&convertOk);
            if (!convertOk)
                break;
            QString username = "";
            pieces.removeAt(MESSAGE::UserId);
            QString msg = pieces.join("");
            //check if user not in chat room
            const int userlistSize = userList.size();
            for (int i = 0; i < userlistSize; i++){
                if (userList[i].userId == userid)
                    username = userList[i].userName;
            }
            if (username != "")
                //send to manager
                emit displayMsg(username,msg);
        }
        break;
    }
    case PROTOCOL_TYPE::ClientJoin:
    {
        // read packet
        QByteArray iv, ciphertext;
        //resize
        iv.resize((int)PROTOCOL::IVSize);
        ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
        //add to qbytearry
        dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
        dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
        //decrypt
        QString text = Custom::decrypt(this->key, iv, ciphertext);
        //split by DELIMITER
        QStringList pieces = text.split(DELIMITER);
        if (pieces.size() == MESSAGE::Section){
            QString textUserid = pieces[MESSAGE::UserId];
            //convert to int
            bool convertOk;
            int userid = textUserid.toInt(&convertOk);
            if (!convertOk)
                break;
            QString username = pieces[MESSAGE::MsgValue];
            //check if user not in chat room
            const int userlistSize = userList.size();
            int i = 0;
            for (; i < userlistSize; i++){
                if (userList[i].userId == userid || userList[i].userName == username)
                    break;
            }
            qDebug() << "I is " << i << " ,username is " << username;
            //if yes, update vector, else reject
            if (i == userlistSize){
                emit userJoin(username);
                userList.append(UserList(userid, username));
            }
        }
        break;
    }
    case PROTOCOL_TYPE::ClientQuit:
    {
        // read packet
        QByteArray iv, ciphertext;
        //resize
        iv.resize((int)PROTOCOL::IVSize);
        ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
        //add to qbytearry
        dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
        dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
        //decrypt
        QString text = Custom::decrypt(this->key, iv, ciphertext);
        //convert to int
        bool convertOk;
        int userid = text.toInt(&convertOk);
        if (!convertOk)
            break;
        //check if user within the chat room
        const int userlistSize = userList.size();
        int i = 0;
        for (; i < userlistSize; i++){
            if (userList[i].userId == userid)
                break;
        }
        //if yes, reject, else update vector
        if (i != userlistSize){
            emit userQuit(userList[i].userName);
            userList.remove(i);
        }
        break;
    }
    case PROTOCOL_TYPE::HostQuit:
    {
        //disconnect procedure
        // read packet
        QByteArray iv, ciphertext;
        //resize
        iv.resize((int)PROTOCOL::IVSize);
        ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
        //add to qbytearry
        dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
        dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
        //decrypt
        QString text = Custom::decrypt(this->key, iv, ciphertext);
        //check nonce
        bool convertOk;
        int sendNonce = text.toInt(&convertOk); //default to 0 if fail
        //check for valid nonce
        if (sendNonce != this->nonce || !convertOk){
            rejectPacket(ERROR::InvalidSessionNonce);
        }else{
            //disconnect
            emit error("Host has left the chatroom.");
        }
        break;
    }
    case PROTOCOL_TYPE::Reject:
    {
        //disconnect procedure as not authenticated
        EType e;
        dataStream >> e;
        switch(e){
        case ERROR::InvalidSize:
        {
            //resend previous packet
            if (this->clientSoc)
                this->clientSoc->write(prevData, prevData.size());
            break;
        }
        case ERROR::InvalidToken:
            //re-init
            initConnection();
            break;
        case ERROR::RoomFull:
        {
            qDebug() << "Room full error";
            //check if already connected (connectionState is true)
            if (!getConnectivity())
                //notify user about room full
                emit error("Chatroom is full.");
            break;
        }
        //illegal packet
        default:
            //ignore
            break;
        }
        break;
    }
    //Illegal operation
    default:
        rejectPacket(ERROR::UnrecognisedPacket);
        break;
    }
}

void ChatClient::initConnection(){
    // send PROTOCOL_TYPE::Init with length of 0
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(DATASTREAMVER);

    stream << TType(PROTOCOL_TYPE::Init)
           << LType(0);

    if (this->clientSoc)
        this->clientSoc->write(data, PROTOCOL::HeaderSize);
    //save for resend
    prevData = QByteArray(data);
}

void ChatClient::sendServerVerify(QString sessionkey,QString verifyIV, QString encAuth){
    /*
     * This function attempt to host chat server with specified group size
     *
     * Input: int groupSize for limiting number of connected user
     * Output: Server start listening at PORT
     */
    // set key
    if (!Custom::setKey(this->key, sessionkey))
        return;
    // convert base64 inputs
    QByteArray iv = QByteArray::fromBase64(verifyIV.toLocal8Bit());
    QByteArray encAuthByte = QByteArray::fromBase64(encAuth.toLocal8Bit());

    // send PROTOCOL_TYPE::ServerVerify
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(DATASTREAMVER);

    // size
    unsigned int totalsize = iv.size() + encAuthByte.size();

    stream << TType(PROTOCOL_TYPE::ServerVerify)
           << LType(totalsize);
    stream.writeRawData(iv.data(), iv.size());
    stream.writeRawData(encAuthByte.data(), encAuthByte.size());

    if (this->clientSoc)
        this->clientSoc->write(data, PROTOCOL::HeaderSize + totalsize);
    //save for resend
    prevData = QByteArray(data);
}
