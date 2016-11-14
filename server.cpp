#include "server.h"

// Constructor & Destructor
Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    connectedUser = {};
    currentId = 1;
}

Server::~Server()
{
    //clear QVector
    connectedUser.clear();
    Q_ASSERT (connectedUser.size() == 0);
    qDebug() << "Closing server";
}

// Function
void Server::startServer()
{
    if(!this->listen(QHostAddress::Any,PORT))
    {
        qDebug() << "Could not start server";
        // timeout of chat group
    }
}

void Server::closeServer()
{
    //removing all connectedUser
    for (int i = 0; i < connectedUser.size(); i++){
        delete connectedUser[i];
    }
    //close this server
    this->close();
}

void Server::rejectPacket(const whiteListObj* wlObj, EType error){
    // send PROTOCOL_TYPE::Reject with fixed 1 byte length
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(DATASTREAMVER);

    stream << TType(PROTOCOL_TYPE::Reject)
           << LType(PROTOCOL::ErrorSize)
           << EType(error);

    wlObj->socket->write(data, PROTOCOL::HeaderSize + PROTOCOL::ErrorSize);
}

void Server::sendPacket(const whiteListObj* wlObj, TType t, QString contents)
{
    qDebug() << "Server::sendPacket -> Sending " << contents;
    // send packet base on t
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(DATASTREAMVER);

    // IV
    QByteArray iv;
    QByteArray ciphertext = Custom::encrypt(wlObj->key, iv, contents);

    // size
    unsigned int totalsize = iv.size() + ciphertext.size();

    // forming packet
    stream << TType(t)
           << LType(totalsize);
    stream.writeRawData(iv.data(), iv.size());
    stream.writeRawData(ciphertext.data(), ciphertext.size());

    wlObj->socket->write(data, PROTOCOL::HeaderSize + totalsize);
}

void Server::processIncomingData(QByteArray data, whiteListObj* wlObj){
    // Debug
    qDebug() << "Server::processIncomingData -> Data size: " << data.size();
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
    if (data.size() != (int)dLength + PROTOCOL::HeaderSize){
        qDebug() << "Different in length, packet size: "
                 << data.size()
                 << " , length value: "
                 << dLength
                 << " , Type of data: "
                 << dType
                 << " , Raw Data: "
                 << data;
        return;
    }

    switch (dType){
    case PROTOCOL_TYPE::Init:
        //send token to connected non-verified user
        if (!wlObj->verified){
            //have not been verified
            QString token = generateToken();
            // set to wlObj
            wlObj->token = token;
            // send raw packet
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream.setVersion(DATASTREAMVER);

            stream << TType(PROTOCOL_TYPE::TokenVerify)
                   << LType(token.size());
            stream.writeRawData(token.toLocal8Bit(), token.size());

            wlObj->socket->write(data, PROTOCOL::HeaderSize + token.size());
        }
        break;
    case PROTOCOL_TYPE::ServerVerify:
    {
        //compare with own secret ***
        QByteArray iv, ciphertext;
        //resize
        iv.resize((int)PROTOCOL::IVSize);
        ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
        //add to qbytearry
        dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
        dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
        //decrypt
        QString text = Custom::decrypt(this->masterKey, iv, ciphertext);
        QByteArray contents = text.toLocal8Bit();
        //token from byte 0 to 15
        QString sendToken = QString::fromLocal8Bit(contents.mid(0,PROTOCOL::TokenSize));
        if (sendToken == wlObj->token){
            //if same set, session key
            qDebug() << "Same";
            //key from byte 16 to dLength
            if (!Custom::setKey( wlObj->key,
                                QString::fromLocal8Bit(contents.mid(PROTOCOL::TokenSize,
                                                                    dLength)
                                                       )
                                )
                )
                rejectPacket(wlObj, ERROR::InvalidToken);
            else{
                //send nonce
                wlObj->nonce = generateNonce();
                sendPacket(wlObj, PROTOCOL_TYPE::SendNonce, QString::number(wlObj->nonce) + DELIMITER + QString::number(wlObj->userId));
            }
        }
        break;
    }
    case PROTOCOL_TYPE::UserDetails:
    {
        //compare with own secret ***
        QByteArray iv, ciphertext;
        //resize
        iv.resize((int)PROTOCOL::IVSize);
        ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
        //add to qbytearry
        dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
        dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
        //decrypt
        QString text = Custom::decrypt(wlObj->key, iv, ciphertext);
        //send to all clientJoin
        qDebug() << "Sending to all client to join by USERDetails";
        sendToAll(QString::number(wlObj->userId) + DELIMITER + text, PROTOCOL_TYPE::ClientJoin);
        //send each one in chatgroup
        int maxSize = connectedUser.size();
        for (int i = 0; i< maxSize; i++){
            if (connectedUser[i]->verified && connectedUser[i]->userId != wlObj->userId)
                sendPacket(wlObj, PROTOCOL_TYPE::ClientJoin,QString::number(connectedUser[i]->userId) + DELIMITER + connectedUser[i]->userName);
        }
        break;
    }
    case PROTOCOL_TYPE::Message:
    {
        //check if verified
        if (wlObj->verified){
            //send to all
            QByteArray iv, ciphertext;
            //resize
            iv.resize((int)PROTOCOL::IVSize);
            ciphertext.resize((int)dLength - PROTOCOL::IVSize); //without iv
            //add to qbytearry
            dataStream.readRawData(iv.data(), (int)PROTOCOL::IVSize);
            dataStream.readRawData(ciphertext.data(), (int)dLength - PROTOCOL::IVSize);
            //decrypt
            QString text = Custom::decrypt(wlObj->key, iv, ciphertext);
            sendToAll(text);
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
        QString text = Custom::decrypt(wlObj->key, iv, ciphertext);
        //convert to int
        bool convertOk;
        int userid = text.toInt(&convertOk);
        if (!convertOk)
            break;
        if (userid == wlObj->userId){
            int currentCount = connectedUser.size();
            wlObj->verified = false;
            wlObj->socket->disconnectFromHost();
            emit updateCount(currentCount - 1);
            sendToAll(text, PROTOCOL_TYPE::ClientQuit);
        }
        break;
    }
    case PROTOCOL_TYPE::Reject:
    {
        EType e;
        dataStream >> e;
        switch(e){
        case ERROR::InvalidSessionNonce:
        {
            //send again
            if (!wlObj->verified)
                sendPacket(wlObj, PROTOCOL_TYPE::TokenVerify, generateToken());
            break;
        }
        //Illegal operation
        default:
            //ignore packet
            break;
        }
        break;
    }
    //Illegal operation
    default:
        //ignore packet
        break;
    }
}

QByteArray Server::convertedData(const QByteArray &data){
    if (data.size() <= 1) //***
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

bool Server::compareIP(const QHostAddress& ip){
    qDebug() << ip.toString();
    if (QHostAddress(ip.toIPv4Address()) == QHostAddress(QHostAddress::LocalHost))
        return true;
    return false;
}

QString Server::generateToken(){
    //generate 8 bytes of random as hex will twice the presentation
    byte token[ PROTOCOL::TokenSize / 2];
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock( token, sizeof(token));
    //convert to hex which will be 16 char/byte
    QByteArray hex((char *)token, sizeof(token));
    return QString(hex.toHex());
}

int Server::generateNonce(){
    //generate 4 bytes of random
    byte nonce[ PROTOCOL::NonceSize ];
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock( nonce, sizeof(nonce) );
    //convert to qbytearray for qdatastream to convert raw binary to int
    QByteArray raw((char*)nonce, sizeof(nonce));
    QDataStream stream(raw);
    int intNonce;
    stream >> intNonce;
    return intNonce;
}

// Slots
void Server::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    //add into whitelist, generate nonce, store IP, username and userID
    QTcpSocket * client = new QTcpSocket(this); //auto delete
    client->setSocketDescriptor(socketDescriptor);

    bool verify = compareIP(client->peerAddress());

    whiteListObj * whitelistStruct = new whiteListObj(client->peerAddress().toString(),
                                                      this->currentId++,
                                                      client,
                                                      verify);
    if (verify){ //own client, skip server verification
        //send nonce
        whitelistStruct->nonce = generateNonce();
        sendPacket(whitelistStruct, PROTOCOL_TYPE::SendNonce, QString::number(whitelistStruct->nonce) + DELIMITER + QString::number(whitelistStruct->userId));
    }
    //append to connectedUser
    connectedUser.append(whitelistStruct);

    //connect signal
    connect(client, &QTcpSocket::readyRead, [this,whitelistStruct]()
    {
        QByteArray data = whitelistStruct->socket->readAll();
        //decrypt data at here, send unencrypted form, check data
        processIncomingData(data, whitelistStruct);
    });
    connect(client, &QTcpSocket::disconnected, [this,whitelistStruct]()
    {
        whitelistStruct->socket->abort();
        if (whitelistStruct->verified)
            sendToAll(QString::number(whitelistStruct->userId),PROTOCOL_TYPE::ClientQuit);
        whitelistStruct->verified = false;
    });

}

void Server::sendToAll(QString data, TType t)
{
    int i = 0;
    int maxSize = connectedUser.size();
    qDebug() << "Server::sendToAll -> Sending to all with " << data;
    qDebug () << maxSize << " , " << i;
    //sending to all connected user;
    for (;i< maxSize; i++){
        if (connectedUser[i] && connectedUser[i]->socket){
            if (connectedUser[i]->verified && connectedUser[i]->socket->state() == QAbstractSocket::ConnectedState){
                qDebug() << "Connected user : " << i;
                sendPacket(connectedUser[i], t, data);
            }
        }
    }
}
