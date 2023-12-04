#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QObject>

#include <concepts>
#include "Window.hpp"
#include <glog/logging.h>

#include <iostream>

class LoginWindow : public QObject {
    // Q_OBJECT

    QWidget loginWidget;
    QVBoxLayout layout;
    QLineEdit userLine;
    QLineEdit passwordLine;
    QPushButton button;

public:
    LoginWindow() : loginWidget{}, layout{&loginWidget} {}
    virtual ~LoginWindow() {}

private slots:
    void pushButton() {
        LOG(INFO) << "HELLO!\n";
    }

public:

    void init() {
        passwordLine.setEchoMode(QLineEdit::Password);
        layout.addWidget(&userLine);
        layout.addWidget(&passwordLine);
        layout.addWidget(&button);
        button.setObjectName("pushButton");

        loginWidget.setWindowTitle("Login");

        loginWidget.show();

        QObject::connect(&button, &QPushButton::clicked, this, &LoginWindow::pushButton);
    }
};