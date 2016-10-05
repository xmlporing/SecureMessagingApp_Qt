#include "chatroom.h"
#include "ui_chatroom.h"

// Constructor & Destructor
Chatroom::Chatroom(QWidget *parent, QString currUser) :
    QDialog(parent),
    ui(new Ui::Chatroom),
    user(currUser)
{
    ui->setupUi(this);

    QDate date = QDate::currentDate();
    ui->textBrowser->append("-- " + date.toString() + " --");
}

Chatroom::~Chatroom()
{
    delete ui;
}

// Display
void Chatroom::displayMsg(QString msg)
{
    QTime time = QTime::currentTime();
    QString timeString = time.toString();

    ui->textBrowser->append(QString("%1 [%2] : %3").arg(user,timeString,msg));
}

// Slots
void Chatroom::on_sendMsgbtn_clicked()
{
    /*
    QTime time = QTime::currentTime();
    //QDateTime datetime = QDateTime::currentDateTime();
    QString timeString = time.toString();

    if (ui->messageEdit->text() != ""){
        ui->textBrowser->append(QString("%1 [%2] : %3").arg(user,timeString,ui->messageEdit->text()));
        //ui->textBrowser->append(&user +"Username: " +ui->messageEdit->text()); alternate way

    }
    ui->messageEdit->clear();
    */
    //displayMsg();
    QString msg;
    msg = ui->messageEdit->text();
    if (msg == ""){
        return;
    }
    ui->messageEdit->clear();

    emit typeMsg(msg);
}

void Chatroom::on_returnBtn_clicked()
{
    this->hide();

    emit leaveRoom();
}
