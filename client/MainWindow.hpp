#pragma once

#include <vector>
#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <glog/logging.h>

#include "Service.hpp"

class MainWindow : public QObject
{
    QPushButton button;
    QVBoxLayout layout;
    QWidget window;
    Service service;
    std::string currentUser_;
    std::string currentPassword_;
    std::vector<std::string> notes_;

private slots:
    void exitButtonPushed() {
        LOG(INFO) << "Exit button is pushed!";
        window.hide();
        ::exit(0);
    }

public:
    MainWindow() {}

    void init(std::string username, std::string password) {
        currentUser_ = std::move(username);
        currentPassword_ = std::move(password);

        layout.addWidget(&button);
        button.setObjectName("pushButton");
        button.setText("Exit");

        window.setLayout(&layout);
        window.setFixedSize(800, 800);

        fetchNotesFromServer();

        QObject::connect(&button, &QPushButton::clicked, this, &MainWindow::exitButtonPushed);
    }

    void show() { window.show(); }

private:
    void fetchNotesFromServer() {
        auto notes = service.getNotes(currentUser_, currentPassword_);
    }
};