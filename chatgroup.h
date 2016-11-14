#ifndef CHATGROUP_H
#define CHATGROUP_H

#include "library.h"

namespace Ui {
class Chatgroup;
}

class Chatgroup : public QDialog
{
    Q_OBJECT

public:
    explicit Chatgroup(QWidget *parent = 0);
    ~Chatgroup();
    void addChatRoom(QString username);
    void removeChatRoom(QString username);

private slots:
    void on_joinButton_clicked();
    void on_logoutButton_clicked();
    void on_createGroupbtn_clicked();

private:
    Ui::Chatgroup *ui;

signals:
    void goMain();
    void joinRoom();
    void makeGroup();
};

#endif // CHATGROUP_H
