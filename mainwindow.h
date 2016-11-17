#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox> //pop up alert
#include "library.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void clearUserDetails();

private:
    Ui::MainWindow *ui;

signals:
    void signIn(QString username, QString pass);
    void createAcc();

private slots:
    void on_signInbtn_clicked();
    void on_createAccbtn_clicked();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
};

#endif // MAINWINDOW_H
