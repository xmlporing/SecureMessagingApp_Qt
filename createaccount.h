#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include "library.h"
#include <QRegularExpression>

namespace Ui {
class CreateAccount;
}

class CreateAccount : public QDialog
{
    Q_OBJECT

public:
    explicit CreateAccount(QWidget *parent = 0);
    ~CreateAccount();

private:
    Ui::CreateAccount *ui;
    bool flag;

private slots:
    void on_backbtn_clicked();
    void on_createAccountbtn_clicked();
    void clearFlag();

signals:
    void registerAcc(QString username, QString password);
    void errorOccur(QString warningmsg);
};

#endif // CREATEACCOUNT_H
