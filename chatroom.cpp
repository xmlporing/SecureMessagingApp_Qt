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
void Chatroom::displayMsg(QString sender, QString msg)
{
    QTime time = QTime::currentTime();
    QString timeString = time.toString();

    ui->textBrowser->append(QString("%1 [%2] : %3").arg(sender,timeString,msg));
}

void Chatroom::addUser(QString username){
    qDebug() << "Adding user " << username;
    ui->listWidget->addItem(username);
}

void Chatroom::removeUser(QString username){
    QList<QListWidgetItem *> result = ui->listWidget->findItems(username, Qt::MatchFixedString);
    if (!(result.size() < 1))
        //remove it
        delete result.at(0);
    qDebug() << "Removing user " << username;
}

// Slots
void Chatroom::on_sendMsgbtn_clicked()
{
    //displayMsg();
    QString msg;
    msg = ui->messageEdit->text();
    if (msg == ""){
        return;
    }
    ui->messageEdit->clear();
    //Debug, sending msg to chat group
    qDebug() << "Chatroom::on_sendMsgbtn_clicked -> Sending " << msg << " to chat group";
    emit typeMsg(msg);
}

void Chatroom::on_returnBtn_clicked()
{
    this->hide();

    emit leaveRoom();
}
