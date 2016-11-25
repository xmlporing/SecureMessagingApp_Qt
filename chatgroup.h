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
    void addChatGroup(QString groupName, int groupSize,
                     int groupCount, QString ip);
    void refreshList();

private slots:
    void on_joinButton_clicked();
    void on_logoutButton_clicked();
    void on_createGroupbtn_clicked();
    void on_refreshButton_clicked();
    void clearFlag();

private:
    Ui::Chatgroup *ui;
    bool flag;

signals:
    void errorOccur(QString warningmsg);
    void goMain();
    void joinRoom(QString groupName, QString ip);
    void makeGroup();
    void updateGroupList();
};

#endif // CHATGROUP_H
