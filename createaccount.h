#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include "library.h"

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

private slots:
    void on_backbtn_clicked();
    void on_createAccountbtn_clicked();

signals:
    void goMain();
};

#endif // CREATEACCOUNT_H
