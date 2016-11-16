#ifndef SSLCLIENT_H
#define SSLCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QDebug>

class SSLClient : public QObject
{
    Q_OBJECT
public:
    explicit SSLClient(QObject *parent = 0);
    void registerAcc(const QString &username, const QString &password);
    void loginToken(const QString &username, const QString &password);
    void logoutAcc(const QString &username, const QString &token);
    void getChatRoom(const QString &username, const QString &token);
    void createChatRoom(const QString& username,const QString& token,
                        const QString& groupName,const int& groupSize,
                        const int& groupCount,const QString& ip);
    void joinChatRoom(const QString& username,const QString& token,
                      const QString& groupName, const QString& hostSessionToken);
    void updateChatRoom(const QString& username, const QString& token, const QString& groupCount);
    void delChatRoom(const QString &username, const QString &token);

private:
    QNetworkAccessManager *manager;
    QNetworkReply * reply;
    QList<QSslError> expectedSslErrors;
    QString baseURL;
    bool debugging;

signals:
    void errorOccur(QString errorMsg);
    void successMsg(QString successMsg);
    void returnLoginToken(const QString &username, const QString &token);
    void logoutSuccess();
    void newChatGroupList(QString groupName, int groupSize,
                          int groupCount, QString ip);
    void createChatRoomSuccess();
    void returnJoinChatRoomToken(QString chatsession,
                                 QString iv, QString encAuth);
    void updateChatRoomSuccess();
    void delChatRoomSuccess();

public slots:
    void recieveRegistration();
    void recieveLoginToken(const QString &username);
    void recieveLogout();
    void recieveChatGroupList();
    void recieveCreateChatRoom();
    void recieveJoinChatRoom();
    void recieveUpdateChatRoom();
    void recieveDelChatRoom();

    void onSslErrors(QNetworkReply*, const QList<QSslError> & );
    void slotError(QNetworkReply::NetworkError code) {
        qDebug() << Q_FUNC_INFO << "Error" << code;
    }
};

#endif // SSLCLIENT_H
