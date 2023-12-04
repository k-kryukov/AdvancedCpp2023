#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QMessageBox>

#include <glog/logging.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openLoginWindow() {
        QWidget loginWidget;
        QVBoxLayout layout(&loginWidget);
        QLineEdit userLine;
        layout.addWidget(&userLine);
        QLineEdit passwordLine;
        passwordLine.setEchoMode(QLineEdit::Password);
        layout.addWidget(&passwordLine);
        loginWidget.setWindowTitle("Login");

        loginWidget.show();

        hide();
    }

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
