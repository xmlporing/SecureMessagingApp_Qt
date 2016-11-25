#include "createaccount.h"
#include "ui_createaccount.h"

// Constructor & Destructor
CreateAccount::CreateAccount(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateAccount)
{
    //default ui setup
    ui->setupUi(this);
    flag = false;
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
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //check for username
    QString username = this->ui->newUserName->text();
    QString password1 = this->ui->newPassword->text();
    QString password2 = this->ui->newCPassword->text();
    //password don't match
    if (password1 != password2){
        emit errorOccur("Password don't match.");
        return;
    }
    //check for valid username
    QRegularExpression userRegex(USERNAME_REGEX);
    QRegularExpressionMatch match = userRegex.match(username);
    if (!match.hasMatch()){
        emit errorOccur("Invalid username entered.");
        return;
    }
    //check for valid password
    QRegularExpression passRegex(PASS_REGEX);
    match = passRegex.match(password1);
    if (!match.hasMatch()){
        emit errorOccur("Invalid password entered.");
        return;
    }
    //register validated user account to web server
    emit registerAcc(username, password1);
}

void CreateAccount::clearFlag(){
    flag = false;
}
