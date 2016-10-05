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
