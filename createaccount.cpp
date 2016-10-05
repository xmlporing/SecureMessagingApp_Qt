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
    this->hide();
    emit goMain();
}
