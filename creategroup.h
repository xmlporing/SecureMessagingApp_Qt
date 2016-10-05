#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include "library.h"

namespace Ui {
class CreateGroup;
}

class CreateGroup : public QDialog
{
    Q_OBJECT

public:
    explicit CreateGroup(QWidget *parent = 0);
    ~CreateGroup();

private:
    Ui::CreateGroup *ui;

private slots:
    void on_backButton_clicked();
    void on_createGroupbtn_clicked();
    void hostLeave();

signals:
    void cancelCreate();
    void createdGroup();
};

#endif // CREATEGROUP_H
