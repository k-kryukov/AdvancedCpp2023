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
#include "MainWindow.hpp"

class LoginWindow : public QObject {
    // Q_OBJECT

    QWidget loginWidget;
    QVBoxLayout layout;
    QLineEdit userLine;
    QLineEdit passwordLine;
    QPushButton button;
    QPushButton registerButton;
    Service service;
    MainWindow* mainWindow;
    RegisterWindow registerWindow;

public:
    LoginWindow(MainWindow* mainWindow)
        : mainWindow{mainWindow}, loginWidget{}, layout{&loginWidget} {}
    virtual ~LoginWindow() {}

private slots:
    void pushButton() {
        auto resp = service.checkCreds(
            userLine.text(),
            passwordLine.text()
        );

        LOG(INFO) << "RESP IS " << resp;
        if (resp / 100 != 2){
            LOG(INFO) << "Creds are not ok!";
            QMessageBox messageBox;
            messageBox.setFixedSize(500,200);
            messageBox.critical(0, "Error", "Wrong creds");
            return;
        }
        else {
            LOG(INFO) << "Creds are ok!";
            loginWidget.hide();

            mainWindow->init(
                userLine.text(),
                passwordLine.text()
            );
            mainWindow->show();
        }
    }

    void pushRegisterButton() {
        LOG(INFO) << "Showing...";
        registerWindow.show();
    }

public:
    void init() {
        passwordLine.setEchoMode(QLineEdit::Password);
        layout.addWidget(&userLine);
        layout.addWidget(&passwordLine);
        layout.addWidget(&button);
        layout.addWidget(&registerButton);
        button.setObjectName("pushButton");
        button.setText("Login");
        registerButton.setObjectName("pushRegisterButton");
        registerButton.setText("Register");

        loginWidget.setWindowTitle("Login");

        loginWidget.setFixedSize(600, 600);
        loginWidget.show();

        QObject::connect(&button, &QPushButton::clicked, this, &LoginWindow::pushButton);
        QObject::connect(&registerButton, &QPushButton::clicked, this, &LoginWindow::pushRegisterButton);
    }
};