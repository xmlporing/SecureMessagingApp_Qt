#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include "library.h"
#include <QHostAddress>

namespace Ui {
class CreateGroup;
}

class CreateGroup : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGroup(QWidget *parent = 0);
    ~CreateGroup();

private:
    Ui::CreateGroup *ui;
    bool flag;

private slots:
    void on_backButton_clicked();
    void on_createGroupbtn_clicked();
    void clearFlag();

signals:
    void errorOccur(QString warningmsg);
    void cancelCreate();
    void createdGroup(QString groupName, int groupSize,
                      int groupCount, QString ip);
};

#endif // CREATEGROUP_H
