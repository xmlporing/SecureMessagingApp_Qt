#include "server.h"

// Constructor & Destructor
Server::Server(QObject *parent) :
    QTcpServer(parent)
{
    connectedUser = {};
    currentId = 1;
    maxCount = 0;
}

Server::~Server()
{
    closeServer();
}

// ******* Public Function *******
bool Server::startServer(int maxUserCount)
{
    /*
     * This function attempt to listen at PORT with specified group size
     *
     * Input: int groupSize for limiting number of connected user
     * Output:
     *      1) True: Server start listening at PORT
     *      2) False: Server failed to listen
     */
    if(!this->listen(QHostAddress::Any,PORT))
    {
        emit error("Unable to host at " + QString::number(PORT));
        // display some error and return to chatgroupui
        return false;
    }
    if (maxUserCount <= 1){
        emit error("Maximum user size are too small.");
        return false;
    }
    //set maxCount
    this->maxCount = maxUserCount;
    this->masterKey = CryptoPP::SecByteBlock(0x00,CryptoPP::AES::DEFAULT_KEYLENGTH);
    //success
    return true;
}

void Server::closeServer()
{
    /*
     * This function will close chat server and remove all allocated objects
     *
     * Input: Nil
     * Output: Nil
     */
    //close this server to prevent more incoming connections
    this->close();
    //update server
    emit delGrp();
    //clearing
    int i = 0;
    //sending "host quit" to all verified and connected user
    for (; i < connectedUser.size(); i++){
        if ( connectedUser.at(i)->socket && connectedUser.at(i)->socket->state() == QAbstractSocket::ConnectedState)
            sendPacket(connectedUser.at(i), PROTOCOL_TYPE::HostQuit, QString::number(connectedUser.at(i)->nonce));
    }
    //removing all object
    for (i = 0; i < connectedUser.size(); i++){
        delete connectedUser[i];
    }
    //clear QVector
    connectedUser.clear();
}

void Server::rejectPacket(const whiteListObj* wlObj, EType error){
    /*
     * This function will send a reject packet of specified type to destinated user
     *
     * Input:
     *      1) const whiteListObj* wlObj -> connected user
     *      2) EType error -> error message code as per network.h
     * Output: Nil
     */
    // check for valid socket
    if(!wlObj->socket)
        return;
    // send PROTOCOL_TYPE::Reject with fixed 1 byte length
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(DATASTREAMVER);

    stream << TType(PROTOCOL_TYPE::Reject)
           << LType(PROTOCOL::ErrorSize)
           << EType(error);

    wlObj->socket->write(data, PROTOCOL::HeaderSize + PROTOCOL::ErrorSize);
    //flush the socket
    wlObj->socket->flush();
}

void Server::sendPacket(whiteListObj* wlObj, TType t, QString contents)
{
    /*
     * This function will send a packet of specified type and related contents
     * to destinated user
     *
     * Input:
     *      1) const whiteListObj* wlObj -> connected user
     *      2) TType t -> type message code as per network.h
     *      3) QString contents -> plaintext that will be converted to encrypted msg
     * Output: Nil
     */
    // check for valid socket
    if(!wlObj->socket)
        return;
    // check if type is message
    if (t == PROTOCOL_TYPE::Message){
        //increase nonce
        Custom::nonceIncrement(wlObj->nonce);
        //adding nonce to message
        contents = QString("%1,%2").arg(QString::number(wlObj->nonce),contents);
    }
    // send packet base on PROTOCOL_TYPE t
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

void Server::setMasterKey(QString key){
    /*
     * This function will attempt to set the master key
     *
     * Input: QString key -> key that is base64 encoded
     * Output: Nil unless there is error setting the key
     */
    if (!Custom::setKey(this->masterKey, key))
        emit error("There is some problem with the login, please logout and try again later.");
}

void Server::setUsername(QString username){
    /*
     * This function will set username
     *
     * Input: QString username
     * Output: Nil
     */
    this->ownUsername = username;
}

// ******* Private function *******

void Server::processIncomingData(QByteArray data, whiteListObj* wlObj){
    /*
     * This function will process incoming data from connected user(s).
     * It validates the packet length with header, and determining on
     * the type of packet recieved and/or decrypt message content,
     * it could be:
     * 1) Forward to every verified user
     * 2) Reply with corresponding packets
     * 3) Reply with reject packet
     * 4) Completely ignore illegal packet
     *
     * Input:
     *      1) QByteArray data -> data that has been sent by connected user
     *      2) const whiteListObj* wlObj -> connected user
     * Output: Nil
     */
    qDebug() << "Server: " + data;
    while (data.size() > 0){
        // check if data is smaller than expected
        if (data.size() < PROTOCOL::HeaderSize)
            return;

        QDataStream dataStream(data);
        dataStream.setVersion(QDataStream::Qt_5_7);

        // check for data
        TType dType = 0x0000;// = static_cast<qint16>data.mid(PROTOCOL::Type,PROTOCOL::TypeSize);
        dataStream >> dType;

        // check for length
        LType dLength = 0x00000000;// = static_cast<qint32>(data.mid(PROTOCOL::Length,PROTOCOL::LengthSize));
        // check for valid length
        dataStream >> dLength;
        if (data.size() < (int)dLength + PROTOCOL::HeaderSize || (int)dLength < 0){
            qDebug() << "Different in length, packet size: "
                     << data.size()
                     << " , length value: "
                     << dLength
                     << " , Type of data: "
                     << dType
                     << " , Raw Data: "
                     << data;
            rejectPacket(wlObj,ERROR::InvalidSize);
            return;
        }
        // check for invalid size length
        if ((int)dLength > MAX_INPUT_CHAR){
            //remove excessive packets
            data.remove(0, (int)dLength + PROTOCOL::HeaderSize);
            //continue without processing
            continue;
        }

        switch (dType){
        case PROTOCOL_TYPE::Init:
            //send token to connected non-verified user
            if (!wlObj->verified){
                //have not been verified
                QString token = generateToken();
                // set to wlObj
                wlObj->token = token;
                // sending raw packet
                // check for valid socket
                if(!wlObj->socket)
                    break;
                // // send PROTOCOL_TYPE::TokenVerify with fixed size token
                QByteArray data2;
                QDataStream stream(&data2, QIODevice::WriteOnly);
                stream.setVersion(DATASTREAMVER);

                stream << TType(PROTOCOL_TYPE::TokenVerify)
                       << LType(token.size());
                stream.writeRawData(token.toLocal8Bit(), token.size());

                wlObj->socket->write(data2, PROTOCOL::HeaderSize + token.size());
                qDebug() << "Init: " + data2;
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
            qDebug() << "decrypted server verify: " + text;
            QStringList pieces = text.split(DELIMITER);
            if (pieces.size() == MESSAGE::Section){
                //check token
                QString sendToken = pieces[MESSAGE::Token];
                if (sendToken == wlObj->token){
                    //update username
                    wlObj->userName = pieces[MESSAGE::Username];
                    //update verification status
                    wlObj->verified = true;
                    //set key
                    if (!Custom::setKey( wlObj->key,pieces[MESSAGE::SessionKey])){
                        rejectPacket(wlObj, ERROR::InvalidToken);
                    }else{
                        //generate nonce
                        wlObj->nonce = generateNonce();
                        //send nonce and allocated id
                        sendPacket(wlObj, PROTOCOL_TYPE::SendNonce,
                                   QString::number(wlObj->nonce) + DELIMITER + QString::number(wlObj->userId) + DELIMITER);
                        //loop to send current userlist
                        for (int i = 0; i < connectedUser.size(); i++){
                            sendPacket(wlObj, PROTOCOL_TYPE::ClientJoin,
                                       DELIMITER + QString::number(connectedUser.at(i)->userId) + DELIMITER + connectedUser.at(i)->userName);
                        }
                        //send to all clientJoin
                        sendToAll(DELIMITER + QString::number(wlObj->userId) + DELIMITER + wlObj->userName  , PROTOCOL_TYPE::ClientJoin);
                    }
                }
            }
            break;
        }
        case PROTOCOL_TYPE::UserDetails:
        {
            //Deprecated since version 1.1
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
                qDebug() << "decrypted message: " + text;
                //split by DELIMITER
                QStringList pieces = text.split(DELIMITER);
                if (pieces.size() >= MESSAGE::Section){
                    //convert to int (nonce)
                    QString textNonce = pieces[MESSAGE::Nonce];
                    bool convertOk;
                    int nonce = textNonce.toInt(&convertOk);
                    if (!convertOk)
                        break;
                    //check if correct nonce
                    if (nonce != wlObj->nonce + 1){
                        //invalid nonce, don't process
                        break;
                    }
                    //increase nonce
                    Custom::nonceIncrement(wlObj->nonce);

                    //convert to int(UserId)
                    QString textUserid = pieces[MESSAGE::UserId];
                    //convert to int
                    textUserid.toInt(&convertOk);
                    if (!convertOk)
                        break;

                    //remove nonce from msg
                    pieces.removeFirst();
                    //remove userId from msg
                    pieces.removeFirst();

                    //msg sent by user
                    QString msg = pieces.join("");
                    //send to all
                    sendToAll(textUserid + DELIMITER + msg);
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
            QString text = Custom::decrypt(wlObj->key, iv, ciphertext);
            qDebug() << "Client quit recieved: " + text;
            //convert to int
            bool convertOk;
            int userid = text.toInt(&convertOk);
            if (!convertOk)
                break;
            if (userid == wlObj->userId){
                int currentCount = connectedUser.size();
                //change verified flag and disconnect
                wlObj->verified = false;
                wlObj->socket->disconnectFromHost();
                //signal
                emit updateCount(currentCount - 1);
                //update all when client quit
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

        //remove of consumed packets
        data.remove(0, (int)dLength + PROTOCOL::HeaderSize);
    }
}

bool Server::compareIP(const QHostAddress& ip){
    /*
     * This function will compare if it is locally connected
     *
     * Input: const QHostAddress& ip -> ip of connected user
     * Output:
     *      1) True: local ip
     *      2) False: non-local ip
     */
    if (QHostAddress(ip.toIPv4Address()) == QHostAddress(QHostAddress::LocalHost))
        return true;
    return false;
}

QString Server::generateToken(){
    /*
     * This function will generate 16 char long random string
     *
     * Input: Nil
     * Output: 16 char of hex encoded string
     */
    //generate 8 bytes of random as hex will twice the presentation
    byte token[ PROTOCOL::TokenSize / 2];
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock( token, sizeof(token));
    //convert to hex which will be 16 char/byte
    QByteArray hex((char *)token, sizeof(token));
    return QString(hex.toHex());
}

int Server::generateNonce(){
    /*
     * This function will generate 32bit random number
     *
     * Input: Nil
     * Output: 32bit number store in int
     */
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
    /*
     * This function will recieve all incoming connection
     * and do connected user check before adding to whitelist
     *
     * Input: qintptr socketDescriptor -> to open a socket
     * Output: 16 char of hex encoded string
     */
    // check for over hitting of maxCount
    if (connectedUser.size() + 1 > maxCount){
        //try to detect and remove disconnected user
        for(int i = 0; i < connectedUser.size();){
            if (connectedUser.at(i)->socket->state() == QTcpSocket::UnconnectedState){
                //delete disconnected user
                delete connectedUser[i];
                //remove it
                connectedUser.remove(i);
            }else{
                //next element
                i++;
            }
        }
        //check if still above count
        if (connectedUser.size() + 1 > maxCount){
            //sending room full packet
            QTcpSocket * client = new QTcpSocket();
            client->setSocketDescriptor(socketDescriptor);
            whiteListObj * whitelistStruct = new whiteListObj(client->peerAddress().toString(),
                                                              this->currentId++,
                                                              client,
                                                              false);
            rejectPacket(whitelistStruct, ERROR::RoomFull);
            delete whitelistStruct;
            //done sending
            return;
        }
    }

    //add into whitelist, generate nonce, store IP, username and userID
    QTcpSocket * client = new QTcpSocket();
    client->setSocketDescriptor(socketDescriptor);

    bool verify = compareIP(client->peerAddress());

    whiteListObj * whitelistStruct = new whiteListObj(client->peerAddress().toString(),
                                                      this->currentId++,
                                                      client,
                                                      verify);
    //append to connectedUser aka whitelist
    connectedUser.append(whitelistStruct);

    if (verify){ //own client, skip server verification
        //set username
        whitelistStruct->userName = this->ownUsername;
        //send nonce
        whitelistStruct->nonce = generateNonce();
        sendPacket(whitelistStruct, PROTOCOL_TYPE::SendNonce, QString::number(whitelistStruct->nonce) + DELIMITER + QString::number(whitelistStruct->userId) + DELIMITER);
        //send that client joined
        sendToAll(DELIMITER + QString::number(whitelistStruct->userId) + DELIMITER + whitelistStruct->userName, PROTOCOL_TYPE::ClientJoin);
    }


    //update chatroom count
    emit updateCount(connectedUser.size());

    //connect signal
    // packet in socket
    connect(client, &QTcpSocket::readyRead, [this,whitelistStruct]()
    {
        //hold all available bytes from packet
        QByteArray data = whitelistStruct->socket->readAll();
        //decrypt data at here, send unencrypted form, check data
        processIncomingData(data, whitelistStruct);
    });
    // user disconnected
    connect(client, &QTcpSocket::disconnected, [this,whitelistStruct]()
    {
        //notify all if verified user leave
        if (whitelistStruct->verified)
            sendToAll(QString::number(whitelistStruct->userId),PROTOCOL_TYPE::ClientQuit);
        //this connection is no longer verified
        whitelistStruct->verified = false;
        //remove unconnected
        connectedUser.removeAll(whitelistStruct);
        //update chatroom count
        emit updateCount(connectedUser.size());
    });

}

void Server::sendToAll(QString data, TType t)
{
    /*
     * This function will encrypt plaintext with data to
     * all verified user of type t
     *
     * Input:
     *      1) QString data -> plaintext to be forwarded to all
     *      2) TType t -> type of data to be forwarded
     * Output: Nil
     */
    //sending to all connected user;
    for (int i = 0;i< connectedUser.size(); i++){
        // valid ptr and has been verified to recieve msg
        if (connectedUser.at(i) && connectedUser.at(i)->verified){
            // valid connected socket
            if ( connectedUser.at(i)->socket && connectedUser.at(i)->socket->state() == QAbstractSocket::ConnectedState){
                sendPacket(connectedUser.at(i), t, data);
            }
        }
    }
}
