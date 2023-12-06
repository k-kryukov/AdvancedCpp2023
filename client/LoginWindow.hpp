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
#include "Connector.hpp"

class LoginWindow : public QObject {
    // Q_OBJECT

    QWidget loginWidget;
    QVBoxLayout layout;
    QLineEdit userLine;
    QLineEdit passwordLine;
    QPushButton button;
    Service service;
    std::hash<std::string> stringHasher;
    MainWindow* mainWindow;

public:
    LoginWindow(MainWindow* mainWindow) : mainWindow{mainWindow}, loginWidget{}, layout{&loginWidget} {}
    virtual ~LoginWindow() {}

private slots:
    void pushButton() {
        auto resp = service.checkCreds(
            userLine.text().toStdString(),
            stringHasher(passwordLine.text().toStdString())
        );

        if (resp) LOG(INFO) << "Creds are ok!";
        else LOG(INFO) << "Creds are not ok!";

        if (resp) {
            loginWidget.hide();
            mainWindow->show();
        }
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