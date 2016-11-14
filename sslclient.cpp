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

    // set base url from config file
    baseURL = "https//somelink.com";
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
        recieveLoginToken(username);
    });

    qDebug() << request.url();
}

void SSLClient::chatRoomToken(const QString &username, const QString &token, const QString &chatroomtoken)
{
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    data["hostSessionToken"] = chatroomtoken;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatroom/join");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveChatroomToken();
    });
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
        registration();
    });
}

void SSLClient::getChatRoom(const QString &username, const QString &token)
{
    //*** reading json
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatroom/list");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveChatGroupList();
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
    QUrl url = QString(baseURL + "/api/chatroom");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->put(request,jsonPost);//(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveChatroomToken(); //***
    });
}

void SSLClient::createChatRoom(const QString &username,
                               const QString &token,
                               const QString &groupName,
                               const QString &groupSize,
                               const QString &groupCount)
{
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    data["groupName"] = token;
    data["groupSize"] = token;
    data["groupCount"] = token;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatroom");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->put(request,jsonPost);//(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveCreateChatRoom();
    });
}

void SSLClient::updateChatRoom(const QString &username, const QString &token, const QString& groupCount)
{
    QJsonObject data;
    data["username"] = username;
    data["token"] = token;
    data["groupCount"] = groupCount;
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString(baseURL + "/api/chatroom");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->put(request,jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        recieveUpdateChatRoom();
    });
}

// ********* slots *********

void SSLClient::recieveLoginToken(const QString &username){
    emit returnLoginToken(username, "fakeTokenHAHAH");
    return;
    /*
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
        QJsonValue token = QJsonObject(itemDoc.object())["token"];
        if (token.isString())
            emit returnLoginToken(username, token.toString());
        else
            emit errorOccur("Invalid login token recieved");
    }else
    // handle non string error or no "token" key error
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
    */
}

void SSLClient::logout(){
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

void SSLClient::recieveChatroomToken(){
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
            emit returnChatRoomToken(chatSessionToken.toString(),
                                     iv.toString(),
                                     encryptedToken.toString());
        }else
            emit errorOccur("Invalid chat room token recieved");
    }else
        // handle non string error or no "token" key error
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::registration(){
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
        emit errorOccur("Registration Successful.");
    }else{
        // registration failed
        emit errorOccur("Registration Failed.");
    }
}

void SSLClient::recieveChatGroupList(){
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
        // going to read json array
        emit newChatGroupList();
        /*
        QJsonValue chatSessionToken = QJsonObject(itemDoc.object())["chatSessionToken"];
        QJsonValue iv = QJsonObject(itemDoc.object())["iv"];
        QJsonValue encryptedToken = QJsonObject(itemDoc.object())["encryptedToken"];
        if (chatSessionToken.isString() && iv.isString() && encryptedToken.isString()){
            emit returnChatRoomToken(chatSessionToken.toString(),
                                     iv.toString(),
                                     encryptedToken.toString());
        }else
            emit errorOccur("Invalid chat room token recieved");
            */
    }else
        // handle non string error or no "token" key error
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::recieveCreateChatRoom(){
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
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::recieveUpdateChatRoom(){
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
        emit errorOccur("Request unsuccessful, " + responseMsg.toString());
}

void SSLClient::onSslErrors(QNetworkReply * reply, const QList<QSslError> & error){
    qDebug() << "Reply: "
             << reply
             << endl
             << "Error: "
             << error;
}
