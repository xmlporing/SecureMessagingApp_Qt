#include "creategroup.h"
#include "ui_creategroup.h"

// Constructor & Destructor
CreateGroup::CreateGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateGroup)
{
    ui->setupUi(this);
    //QString user = (MainWindow)UIparent->getUsername;
}

CreateGroup::~CreateGroup()
{
    delete ui;
}

// Slots
void CreateGroup::on_backButton_clicked()
{
    this->hide();

    emit cancelCreate();
}

void CreateGroup::on_createGroupbtn_clicked()
{
    this->hide();

    emit createdGroup();
}

void CreateGroup::hostLeave()
{
    this->hide();

    emit cancelCreate();
}
