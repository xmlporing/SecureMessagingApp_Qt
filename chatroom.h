#ifndef CHATROOM_H
#define CHATROOM_H

#include "library.h"

namespace Ui {
class Chatroom;
}

class Chatroom : public QDialog
{
    Q_OBJECT

public:
    explicit Chatroom(QWidget *parent = 0, QString currUser = NULL);
    ~Chatroom();
    void displayMsg(QString sender, QString msg);
    void addUser(QString username);
    void removeUser(QString username);

private:
    Ui::Chatroom *ui;
    QString user;
    int count;

private slots:
    void on_sendMsgbtn_clicked();
    void on_returnBtn_clicked();
    void clearCount();

signals:
    void errorOccur(QString errMsg);
    void leaveRoom();
    void typeMsg(QString msg);
};

#endif // CHATROOM_H
