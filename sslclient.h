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
    void runClient(const QString &, const QString &);

private:
    QNetworkAccessManager *manager;
    QNetworkReply * reply;
    QList<QSslError> expectedSslErrors;

signals:
    void returnToken(const QString &username, const QString &token);

public slots:
    void finishedReply(const QString &username);
    void onSslErrors(QNetworkReply*, const QList<QSslError> & );
    void slotError(QNetworkReply::NetworkError code) {
        qDebug() << Q_FUNC_INFO << "Error" << code;
    }
};

#endif // SSLCLIENT_H
