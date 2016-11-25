#include "creategroup.h"
#include "ui_creategroup.h"

// Constructor & Destructor
CreateGroup::CreateGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateGroup)
{
    //default ui setup
    ui->setupUi(this);
    //set inputmask
    ui->groupSize->setInputMask("00"); //max 99 connections
    ui->ip->setInputMask("000.000.000.000"); //ip mask
    flag = false;
}

CreateGroup::~CreateGroup()
{
    delete ui;
}

// Slots
void CreateGroup::on_backButton_clicked()
{
    /*
     * This function show chat group UI
     *
     * Signal from: back button clicked
     * Input: Nil
     * Output: Show chat group UI
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //return from creation
    emit cancelCreate();
}

void CreateGroup::on_createGroupbtn_clicked()
{
    /*
     * This function attempt to host chat room
     *
     * Signal from: Create Group button clicked
     * Input: Nil
     * Output:
     *      1) Show chat room UI when successfully hosted
     *      2) Display error msg when unsuccessfully host chat server
     *      3) Display error msg when unsuccessfully communicate with web server
     */
    //prevent spam
    if (flag)
        return;
    flag = true;
    QTimer::singleShot(TIME_PREVENT_SPAM, this, SLOT(clearFlag()));
    //get from inputs
    QString groupName = this->ui->groupName->text();
    int groupSize = this->ui->groupSize->text().toInt();
    int groupCount = 1; //user will auto connect but will not be updated to web server
    QString ip = this->ui->ip->text();
    //validate inputs
    // ip, invalid ip
    QHostAddress myIP;
    if (!myIP.setAddress(ip)){
        emit errorOccur("Invalid IP address");
        return;
    }
    // groupName, empty or too large groupName
    if (groupName == "" || groupName.size() >= MAX_INPUT_CHAR){
        emit errorOccur("Invalid group name (Cannot be empty or over 254 characters");
        return;
    }
    // groupSize, less than 2 to have form a group
    if (groupSize < 2){
        emit errorOccur("Invalid group size (Requires more than 2)");
        return;
    }
    //emit validated inputs
    emit createdGroup(groupName, groupSize, groupCount, ip);
}

void CreateGroup::clearFlag(){
    flag = false;
}
