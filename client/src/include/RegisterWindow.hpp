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
    QWidget widget;
    QPushButton button;
    QVBoxLayout layout;
    QLineEdit userLine;
    QLineEdit passwordLine;
    Service service;

public:
    RegisterWindow() : widget{} { init(); }
    virtual ~RegisterWindow() {}

    void show() { widget.setFixedSize(600, 600); widget.show(); }

private slots:
    void pushButton() {
        LOG(INFO) << "Register!";
        widget.hide();

        auto res = service.createUser(userLine.text(), passwordLine.text());
        QMessageBox messageBox;
        messageBox.setFixedSize(500,200);
        if (res / 100 != 2) {
            DLOG(INFO) << "res is " << res;
            messageBox.critical(&widget, "Error", "An error has occured: usually it means that user already exists!");
        }
        else
            messageBox.information(&widget, "OK!", "");
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

        widget.setWindowTitle("Register window");
        widget.setLayout(&layout);

        QObject::connect(&button, &QPushButton::clicked, this, &RegisterWindow::pushButton);
    }
};