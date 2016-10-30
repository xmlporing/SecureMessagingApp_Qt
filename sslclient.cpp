#include "sslclient.h"

SSLClient::SSLClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    // debug ssl errors
    // connect(manager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(onSslErrors(QNetworkReply*, const QList<QSslError> &)));

    // ignore self-signed cert issues
    QList<QSslCertificate> cert = QSslCertificate::fromPath(":/cert/apache.crt");
    expectedSslErrors.append(QSslError(QSslError::SelfSignedCertificate, cert.at(0)));
    expectedSslErrors.append(QSslError(QSslError::HostNameMismatch,cert.at(0)));
    expectedSslErrors.append(QSslError(QSslError::CertificateUntrusted ,cert.at(0)));
}

void SSLClient::runClient(const QString &username, const QString &password)
{
    QJsonObject data;
    data["username"] = QString(username);
    data["password"] = QString(password);
    QByteArray jsonPost = QJsonDocument(data).toJson(QJsonDocument::Compact);

    QNetworkRequest request;
    QUrl url = QString("<URL place here>");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    reply = manager->post(request, jsonPost);
    reply->ignoreSslErrors(expectedSslErrors);
    connect(reply, &QNetworkReply::finished, [this, username](){
        finishedReply(username);
    });
    //connect(reply, SIGNAL(finished()), this, SLOT(finishedReply()));
    //connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(slotError(QNetworkReply::NetworkError)));
}

void SSLClient::finishedReply(const QString &username){
    if (reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    // sample return value {"token":"<some token value>"}
    QJsonDocument itemDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonValue token = QJsonObject(itemDoc.object())["token"];
    if (token.isString()){
        emit returnToken(username, token.toString());
    }
    // handle non string error or no "token" key error
}

void SSLClient::onSslErrors(QNetworkReply * reply, const QList<QSslError> & error){
    qDebug() << "Reply: " << reply << endl << "Error: " << error;
}
