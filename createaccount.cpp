#include "createaccount.h"
#include "ui_createaccount.h"

// Constructor & Destructor
CreateAccount::CreateAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAccount)
{
    //default ui setup
    ui->setupUi(this);
}

CreateAccount::~CreateAccount()
{
    delete ui;
}

// Slots
void CreateAccount::on_backbtn_clicked()
{
    /*
     * This function close registration window
     *
     * Signal from: Back button clicked
     * Input: Nil
     * Output: Hide registration window
     */
    this->hide();
}

void CreateAccount::on_createAccountbtn_clicked()
{
    /*
     * This function try to register a new account from textfields
     *
     * Signal from: Create button clicked
     * Input: Nil
     * Output:
     *      1) Error msg for invalid username/password
     *      2) Success msg for successful registration
     */
    QString username = this->ui->newUserName->text();
    QString password1 = this->ui->newPassword->text();
    QString password2 = this->ui->newCPassword->text();
    QRegularExpression regex("^[a-zA-Z0-9]{8,16}$");
    if (password1 != password2){
        emit errorOccur("Password don't match.");
        return;
    }
    QRegularExpressionMatch match = regex.match(password1);
    if (!match.hasMatch()){
        emit errorOccur("Invalid password entered.");
        return;
    }
    match = regex.match(username);
    if (!match.hasMatch()){
        emit errorOccur("Invalid username entered.");
        return;
    }
    //register validated user account to web server
    emit registerAcc(username, password1);
}
