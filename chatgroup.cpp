#include "chatgroup.h"
#include "ui_chatgroup.h"

// Constructor & Destructor
Chatgroup::Chatgroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chatgroup)
{
    ui->setupUi(this);
    QDateTime datetime = QDateTime::currentDateTime();
    ui->dateTimeHere->setText(datetime.toString());
}

Chatgroup::~Chatgroup()
{
    delete ui;
}

void Chatgroup::addChatRoom(QString username){
    qDebug() << "Adding user " << username;
    //ui->listWidget->addItem(username);
}

void Chatgroup::removeChatRoom(QString username){
   // QList<QListWidgetItem *> result = ui->listWidget->findItems(username, Qt::MatchFixedString);
    //if (!(result.size() < 1))
        //remove it
        //delete result.at(0);
    qDebug() << "Removing user " << username;
}

// Slots
void Chatgroup::on_joinButton_clicked()
{
    this->hide();

    //authentication to socket here

    // then join room
    emit joinRoom();
}

void Chatgroup::on_logoutButton_clicked()
{
    this->hide();

    // clear authentication

    emit goMain();
}

void Chatgroup::on_createGroupbtn_clicked()
{
    this->hide();

    emit makeGroup();
}
