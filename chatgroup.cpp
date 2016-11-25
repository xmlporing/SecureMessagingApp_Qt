#include "chatgroup.h"
#include "ui_chatgroup.h"

// Constructor & Destructor
Chatgroup::Chatgroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chatgroup)
{
    //default setup ui
    ui->setupUi(this);
    //set single selection
    ui->listChatgroup->setSelectionMode(QAbstractItemView::SingleSelection);
    flag = false;
}

Chatgroup::~Chatgroup()
{
    delete ui;
}

void Chatgroup::addChatGroup(QString groupName, int groupSize,
                             int groupCount, QString ip){
    /*
     * This function add chat group to chat group list
     *
     * Signal from: SSLClient::recieveChatGroupList -> SSLClient::newChatGroupList
     * Input: Nil
     * Output: Chat group entry [groupCount/groupSize] groupName and Qt::WhatsThisRole store ip
     */
    //add new entry
    QString item = QString("[%1/%2]%3").arg(QString::number(groupCount),
                                                 QString::number(groupSize),
                                                 groupName);
    this->ui->listChatgroup->addItem(item);
    //retrieve the entry
    QListWidgetItem* itemPtr = this->ui->listChatgroup->item(this->ui->listChatgroup->count()-1);
    //set ip into data
    itemPtr->setData(Qt::WhatsThisRole, ip);
}

void Chatgroup::refreshList(){
    /*
     * This function clear chat group list
     *
     * Signal from: Chatgroup::on_refreshButton_clicked / Refresh button clicked
     * Input: Nil
     * Output: Clear current chat group list and get updated list
     */
    //delete the list one by one
    while(this->ui->listChatgroup->count()>0)
    {
        //retrieve from front
        QListWidgetItem* tempPtr = this->ui->listChatgroup->takeItem(0);
        //delete it
        delete tempPtr;
    }
    //signal for SSLClient to retrieve chat group list
    emit updateGroupList();
}

// Slots
void Chatgroup::on_joinButton_clicked()
{
    /*
     * This function attempt to join chat room with selected group name and ip
     *
     * Signal from: Join button clicked
     * Input: Nil
     * Output:
     *      1) Show chatroom UI when successfully connected to chat room
     *      2) Display error msg when no chat room selected
     *      3) Display error msg when unsuccessfully connect to chat room
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //check for selected
    QList<QListWidgetItem*> selected = this->ui->listChatgroup->selectedItems();
    //check if there is selected
    if (selected.size() != 1){
        emit errorOccur("No chatroom selected");
        return;
    }
    //extract group name
    QString groupEntryString = selected.at(0)->text();
    //Get group name eg. "[1/10]AHHSASJH come bro!" -> 0:"[1/10", 1:"AHHSASJH come bro!", 2:""
    QString groupName = groupEntryString.section("]",1);;
    //Get ip from data
    QString ip = selected.at(0)->data(Qt::WhatsThisRole).toString();
    //debug
    qDebug() << "Group name: " << groupName << " , IP: " << ip;
    //attempt to join room, set to client
    emit joinRoom(groupName, ip);
}

void Chatgroup::on_logoutButton_clicked()
{
    /*
     * This function attempt to log out and show main UI
     *
     * Signal from: Logout button clicked
     * Input: Nil
     * Output: Attempt to clear session token in web server (logout)
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    emit goMain();
}

void Chatgroup::on_createGroupbtn_clicked()
{
    /*
     * This function show create group UI
     *
     * Signal from: Create Group button clicked
     * Input: Nil
     * Output: Show create group UI
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //make group
    emit makeGroup();
}

void Chatgroup::on_refreshButton_clicked()
{
    /*
     * This function refresh the chat group list
     *
     * Signal from: Refresh button clicked
     * Input: Nil
     * Output: Clear current chat group list and get updated list
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //refresh list
    refreshList();
}

void Chatgroup::clearFlag(){
    flag = false;
}
