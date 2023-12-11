#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QObject>

#include <concepts>
#include <iostream>
#include <glog/logging.h>

#include "Window.hpp"
#include "Service.hpp"
#include "RegisterWindow.hpp"

class RegisterWindow : public QObject {
    QWidget registerWidget;
    QPushButton button;
    QVBoxLayout layout;
    QLineEdit userLine;
    QLineEdit passwordLine;
    Service service;

public:
    RegisterWindow() : registerWidget{} { init(); }
    virtual ~RegisterWindow() {}

    void show() { registerWidget.show(); }

private slots:
    void pushButton() {
        LOG(INFO) << "Register!";

        auto res = service.createUser(userLine.text().toStdString(), passwordLine.text().toStdString());
        QMessageBox messageBox;
        messageBox.setFixedSize(500,200);
        if (!res)
            messageBox.critical(0, "Error", "An error has occured !");
        else
            messageBox.information(0, "OK!", "");
    }

public:
    void init() {
        passwordLine.setEchoMode(QLineEdit::Password);
        layout.addWidget(&userLine);
        layout.addWidget(&passwordLine);
        layout.addWidget(&button);
        button.setObjectName("pushButton");

        userLine.setText("Login");
        passwordLine.setText("Password");

        button.setText("Register!");

        registerWidget.setWindowTitle("Register window");
        registerWidget.setLayout(&layout);

        QObject::connect(&button, &QPushButton::clicked, this, &RegisterWindow::pushButton);
    }
};