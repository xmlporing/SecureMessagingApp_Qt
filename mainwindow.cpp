#include "mainwindow.h"
#include "ui_mainwindow.h"

// Constructor & Destructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //default ui setup
    ui->setupUi(this);
    flag = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearUserDetails()
{
    /*
     * This function clear user input fields
     *
     * Input: Nil
     * Output: Cleared username and password input fields
     */
    ui->userName->clear();
    ui->passWord->clear();
}

// Slot
void MainWindow::on_signInbtn_clicked()
{
    /*
     * This function attempt to signal login in web server
     *
     * Signal from: Sign in button clicked
     * Input: Nil
     * Output: Attempt to login with user inputs
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //get from user inputs
    QString username = ui->userName->text();
    QString pass = ui->passWord->text();
    //check for valid username
    QRegularExpression userRegex(USERNAME_REGEX);
    QRegularExpressionMatch match = userRegex.match(username);
    if (!match.hasMatch()){
        emit errorOccur("Invalid username entered.");
        return;
    }
    //check for valid password
    QRegularExpression passRegex(PASS_REGEX);
    match = passRegex.match(pass);
    if (!match.hasMatch()){
        emit errorOccur("Invalid password entered.");
        return;
    }
    //sign in
    emit signIn(username,pass);
}

void MainWindow::on_createAccbtn_clicked()
{
    /*
     * This function show registration UI
     *
     * Signal from: create account button clicked
     * Input: Nil
     * Output: Show registration UI
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //show registration UI
    emit createAcc();
}

void MainWindow::on_actionExit_triggered() {
    /*
     * This function quit the application
     *
     * Signal from: File -> Exit from toolbar
     * Input: Nil
     * Output: Nil
     */
    QApplication::quit();
}

void MainWindow::on_actionAbout_triggered() {
    /*
     * This function display the application details
     *
     * Signal from: Help -> About from toolbar
     * Input: Nil
     * Output: Nil
     */
    QMessageBox msgBox;
    msgBox.setText("Secure Messaging Application created by Team 8 for ICT3103.");
    msgBox.exec();
}

void MainWindow::clearFlag(){
    flag = false;
}
