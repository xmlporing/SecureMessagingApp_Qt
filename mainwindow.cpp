#include "mainwindow.h"
#include "ui_mainwindow.h"

// Constructor & Destructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Slot
void MainWindow::on_signInbtn_clicked()
{
    QString username = ui->userName->text();
    QString pass = ui->passWord->text();
    //sign in
    emit signIn(username,pass);
}

void MainWindow::on_createAccbtn_clicked()
{
    //create account
    emit createAcc();
}
