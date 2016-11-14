#ifndef SSLCLIENT_H
#define SSLCLIENT_H

#include <QObject>
#include <QtNetwork>
#include <QDebug>

class SSLClient : public QObject
{
    Q_OBJECT
public:
    explicit SSLClient(QObject *parent = 0);
    void loginToken(const QString &username, const QString &password);
    void logoutAcc(const QString &username, const QString &token);
    void chatRoomToken(const QString& username, const QString& token, const QString& chatroomtoken);
    void registerAcc(const QString &username, const QString &password);
    void getChatRoom(const QString &username, const QString &token);
    void delChatRoom(const QString &username, const QString &token);
    void createChatRoom(const QString& username,
                        const QString& token,
                        const QString& groupName,
                        const QString& groupSize,
                        const QString& groupCount);
    void updateChatRoom(const QString& username, const QString& token, const QString& groupCount);

private:
    QNetworkAccessManager *manager;
    QNetworkReply * reply;
    QList<QSslError> expectedSslErrors;
    QString baseURL;

signals:
    void errorOccur(QString errorMsg);
    void returnLoginToken(const QString &username, const QString &token);
    void returnChatRoomToken(const QString &chatsession, const QString &iv, const QString &encAuth);
    void logoutSuccess();
    void newChatGroupList(); //*** havent read json
    //del
    void createChatRoomSuccess();
    void updateChatRoomSuccess();

public slots:
    void recieveLoginToken(const QString &username);
    void logout();
    void recieveChatroomToken();
    void registration();
    void recieveChatGroupList(); //*** havent read json
    //del
    void recieveCreateChatRoom();
    void recieveUpdateChatRoom();

    void onSslErrors(QNetworkReply*, const QList<QSslError> & );
    void slotError(QNetworkReply::NetworkError code) {
        qDebug() << Q_FUNC_INFO << "Error" << code;
    }
};

#endif // SSLCLIENT_H
