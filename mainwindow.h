#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox> //pop up alert

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
};

#endif // MAINWINDOW_H
