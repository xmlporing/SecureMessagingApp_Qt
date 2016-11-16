#include "sslclient.h"

SSLClient::SSLClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    // debug ssl errors
    //connect(manager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(onSslErrors(QNetworkReply*, const QList<QSslError> &)));

    // ignore self-signed cert issues
    QList<QSslCertificate> cert = QSslCertificate::fromPath(":/cert/apache.crt");
    expectedSslErrors.append(QSslError(QSslError::SelfSignedCertificate, cert.at(0)));
    expectedSslErrors.append(QSslError(QSslError::HostNameMismatch,cert.at(0)));
    expectedSslErrors.append(QSslError(QSslError::CertificateUntrusted ,cert.at(0)));

    // read from config file
    QSettings settings("config.ini",QSettings::IniFormat);
    qDebug() << settings.fileName();
    baseURL = settings.value("server", "").toString();
    if (baseURL == ""){
        settings.setValue("server", "https://changetoyourdomain.com");
        baseURL = settings.value("server", "").toString();
    }

    //debugging 1) locally(true) 2) network(false)
    debugging = false;
}

void SSLClient::registerAcc(const QString &username, const QString &password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/register");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveRegistration();
    });
}

void SSLClient::loginToken(const QString &username, const QString &password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/login");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveLoginToken(username);
    });

    qDebug() << request.url();
}

void SSLClient::logoutAcc(const QString &username, const QString &token)
{
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/logout");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveLogout();
    });

    qDebug() << request.url();
}

void SSLClient::getChatRoom(const QString &username, const QString &token)
{
    //*** reading json
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatgroup/list");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveChatGroupList();
    });
}

void SSLClient::createChatRoom(const QString &username, const QString &token,
                               const QString &groupName, const int &groupSize,
                               const int &groupCount, const QString &ip)
{
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    data["groupName"] = groupName;
    data["groupSize"] = groupSize;
    data["groupCount"] = groupCount;
    data["ip"] = ip;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatgroup");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->put(request,jsonPost);//(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveCreateChatRoom();
    });
}

void SSLClient::joinChatRoom(const QString &username, const QString &token,
                             const QString &groupName, const QString &hostSessionToken)
{
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    data["groupName"] = groupName;
    data["hostSessionToken"] = hostSessionToken;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatgroup/join");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveJoinChatRoom();
    });
}

void SSLClient::updateChatRoom(const QString &username,
                               const QString &token, const QString& groupCount)
{
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    data["groupCount"] = groupCount;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatgroup");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->put(request,jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveUpdateChatRoom();
    });
}

void SSLClient::delChatRoom(const QString &username, const QString &token)
{
    //*** QNetworkAccessManager::deleteResource only have request
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatgroup/delete");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->put(request,jsonPost);//(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveDelChatRoom(); //***
    });
}

// ********* slots *********
void SSLClient::recieveRegistration(){
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        emit errorOccur("Error reaching server");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    if (success.isString() && success.toString() == "success"){
        emit successMsg("Registration Successful.");
    }else{
        // registration failed
        emit errorOccur("Registration Failed.");
    }
}

void SSLClient::recieveLoginToken(const QString &username){
    if (debugging){
        emit returnLoginToken(username, "testingtoken");
        return;
    }
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        emit errorOccur("Error reaching server");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    QJsonValue responseMsg = QJsonObject(itemDoc.object())["responseMessage"];
    if (success.isString() && success.toString() == "success"){
        QJsonValue token = QJsonObject(itemDoc.object())["token"];
        if (token.isString())
            emit returnLoginToken(username, token.toString());
        else
            emit errorOccur("Invalid login token recieved");
    }else
    // handle non string error or no "token" key error
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::recieveLogout(){
    if (debugging){
        emit logoutSuccess();
        return;
    }
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        emit errorOccur("Error reaching server");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    if (success.isString() && success.toString() == "success"){
        // utilising errorOccur to convey success
        emit logoutSuccess();
    }else{
        // registration failed
        emit errorOccur("Logout Unsuccessful.");
    }
}

void SSLClient::recieveChatGroupList(){
    if (debugging){
        emit newChatGroupList("GroupName1",
                10,
                3,
                "192.1.1.1");
        emit newChatGroupList("GroupName2",
                2,
                1,
                "192.1.1.2");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    QJsonValue responseMsg = QJsonObject(itemDoc.object())["responseMessage"];
    if (success.isString() && success.toString() == "success"){
        // going to read json array
        QJsonObject object = itemDoc.object();
        QJsonValue chatGroupsRawValue = object.value("chatGroups");
        if (chatGroupsRawValue.isArray()){
            QJsonArray chatGroupsArray = chatGroupsRawValue.toArray();
            for (const QJsonValue& chatGroup : chatGroupsArray){
                // convert to QJsonObject
                QJsonObject loopObj = chatGroup.toObject();
                // emit one by one
                if (loopObj["groupName"].isString() &&
                        loopObj["groupSize"].isDouble() &&
                        loopObj["groupCount"].isDouble() &&
                        loopObj["ip"].isString())
                {
                    emit newChatGroupList(loopObj["groupName"].toString(),
                            loopObj["groupSize"].toInt(),
                            loopObj["groupCount"].toInt(),
                            loopObj["ip"].toString());
                }
            }
        }else{
            emit errorOccur("Currently there is no online chat room.");
        }
    }else
        // handle non string error or no "token" key error
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::recieveCreateChatRoom(){
    if (debugging){
        emit createChatRoomSuccess();
        return;
    }
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        emit errorOccur("Error reaching server");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    QJsonValue responseMsg = QJsonObject(itemDoc.object())["responseMessage"];
    if ((success.isString() && success.toString() == "success") || true){
        emit createChatRoomSuccess();
    }else
    // handle non string error or no "token" key error
        emit errorOccur("Create Chatroom Failed : Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::recieveJoinChatRoom(){
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        emit errorOccur("Error reaching server");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    QJsonValue responseMsg = QJsonObject(itemDoc.object())["responseMessage"];
    if (success.isString() && success.toString() == "success"){
        QJsonValue chatSessionToken = QJsonObject(itemDoc.object())["chatSessionToken"];
        QJsonValue iv = QJsonObject(itemDoc.object())["iv"];
        QJsonValue encryptedToken = QJsonObject(itemDoc.object())["encryptedToken"];
        if (chatSessionToken.isString() && iv.isString() && encryptedToken.isString()){
            emit returnJoinChatRoomToken(chatSessionToken.toString(),
                                     iv.toString(),
                                     encryptedToken.toString());
        }else
            emit errorOccur("Invalid chat room token recieved");
    }else
        // handle non string error or no "token" key error
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::recieveUpdateChatRoom(){
    if (debugging){
        emit updateChatRoomSuccess();
        return;
    }
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        emit errorOccur("Error reaching server");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    QJsonValue responseMsg = QJsonObject(itemDoc.object())["responseMessage"];
    if ((success.isString() && success.toString() == "success") || true){
        emit updateChatRoomSuccess();
    }else
    // handle non string error or no "token" key error
        emit errorOccur("Update to webserver failed : Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::recieveDelChatRoom(){
    if (debugging){
        emit delChatRoomSuccess();
        return;
    }
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        emit errorOccur("Error reaching server");
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue success = QJsonObject(itemDoc.object())["response"];
    QJsonValue responseMsg = QJsonObject(itemDoc.object())["responseMessage"];
    if ((success.isString() && success.toString() == "success") || true){
        emit delChatRoomSuccess();
    }else
    // handle non string error or no "token" key error
        emit errorOccur("Update server failed : Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::onSslErrors(QNetworkReply * reply, const QList<QSslError> & error){
    qDebug() << "Reply: "
             << reply
             << endl
             << "Error: "
             << error;
}
