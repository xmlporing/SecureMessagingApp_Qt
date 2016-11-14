#include "createaccount.h"
#include "ui_createaccount.h"

// Constructor & Destructor
CreateAccount::CreateAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAccount)
{
    ui->setupUi(this);
}

CreateAccount::~CreateAccount()
{
    delete ui;
}

// Slots
void CreateAccount::on_backbtn_clicked()
{
    this->hide();
    emit goMain();
}

void CreateAccount::on_createAccountbtn_clicked()
{
    QString username = this->ui->newUserName->text();
    QString password1 = this->ui->newPassword->text();
    QString password2 = this->ui->newCPassword->text();
    QRegularExpression regex("^[a-zA-Z0-9]{8,16}$");
    if (password1 != password2)
        return;
    QRegularExpressionMatch match = regex.match(password1);
    if (!match.hasMatch())
        return;
    match = regex.match(username);
    if (!match.hasMatch())
        return;
    emit registerAcc(username, password1);
}
