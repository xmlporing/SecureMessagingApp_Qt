#include "chatroom.h"
#include "ui_chatroom.h"

// Constructor & Destructor
Chatroom::Chatroom(QWidget *parent, QString currUser) :
    QDialog(parent),
    ui(new Ui::Chatroom),
    user(currUser)
{
    //default setup ui
    ui->setupUi(this);
    //set current time
    QDate date = QDate::currentDate();
    ui->textBrowser->append("-- " + date.toString() + " --");
    //init data
    count = 0;
    user = "";
    //set for non selection
    ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

Chatroom::~Chatroom()
{
    delete ui;
}

// Display
void Chatroom::displayMsg(QString sender, QString msg)
{
    /*
     * This function will format and display message
     *
     * Input:
     *      1) QString sender -> user that send msg
     *      2) QString msg -> recieved msg
     * Output: Nil
     */
    //get current time
    QTime time = QTime::currentTime();
    QString timeString = time.toString();
    //output formatted msg with timestamp
    ui->textBrowser->append(QString("%1 [%2] : %3").arg(sender,timeString,msg));
}

void Chatroom::addUser(QString username){
    /*
     * This function will update connected user list
     *
     * Input: QString username -> new connected user
     * Output: Nil
     */
    qDebug() << "Adding user " << username;
    ui->listWidget->addItem(username);
}

void Chatroom::removeUser(QString username){
    /*
     * This function will update disconnected user list
     *
     * Input: QString username -> disconnected user
     * Output: Nil
     */
    QList<QListWidgetItem *> result = ui->listWidget->findItems(username, Qt::MatchFixedString);
    if (!(result.size() < 1))
        //remove it
        delete result.at(0);
    qDebug() << "Removing user " << username;
}

// Slots
void Chatroom::on_sendMsgbtn_clicked()
{
    /*
     * This function will attempt to send message to host
     * from message text field
     *
     * Signal: Send msg button clicked or "Enter" keypress
     * Input: Nil
     * Output: Nil
     */
    QString msg = ui->messageEdit->text();
    if (msg == "" || msg.size() >= MAX_INPUT_CHAR){
        emit errorOccur("Invalid message");
        return;
    }
    if (this->count >= MSG_PER_30_SEC){
        emit errorOccur("Please wait before sending again (30s)");
        return;
    }
    //valid message increase temp count
    this->count++;
    QTimer::singleShot(TIME_30_SEC, this, SLOT(clearCount()));
    //clear the message block
    ui->messageEdit->clear();
    //Debug, sending msg to chat group
    qDebug() << "Chatroom::on_sendMsgbtn_clicked -> Sending " << msg << " to chat group";
    emit typeMsg(msg);
}

void Chatroom::on_returnBtn_clicked()
{
    /*
     * This function will attempt to leave chat room
     *
     * Signal: Back button clicked
     * Input: Nil
     * Output: Nil
     */
    emit leaveRoom();
}

void Chatroom::clearCount(){
    /*
     * This function will attempt to lower the count
     *
     * Signal: When message are send
     * Input: Nil
     * Output: Nil
     */
    if (this->count > 0){
        this->count--;
    }
}
