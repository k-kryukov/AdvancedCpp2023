#pragma once

#include <vector>
#include <thread>

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <glog/logging.h>

#include "Service.hpp"

class MainWindow : public QObject
{
    QPushButton exitButton;
    QPushButton refreshButton;
    QVBoxLayout layout;
    QWidget window;
    Service service;
    std::string currentUser_;
    std::string currentPassword_;
    std::vector<std::string> notes_;
    std::vector<QLabel*> labels_;

public:
    MainWindow() {}

    void emplaceNotes(std::vector<std::string> notes) {
        for (auto label : labels_) {
            layout.removeWidget(label);
        }

        std::for_each(labels_.begin(), labels_.end(), [] (auto widget) { delete widget; });
        labels_.clear();

        for (auto&& note : notes) {
            QLabel *label = new QLabel(note.data(), &window);
            layout.addWidget(label);
            labels_.push_back(label);
        }
    }

private:
    auto fetchNotesFromServer() {
        return service.getNotes(currentUser_, currentPassword_);
    }

private slots:
    void exitButtonPushed() {
        LOG(INFO) << "Exit button is pushed!";
        window.hide();
        ::exit(0);
    }

    void refreshButtonPushed() {
        LOG(INFO) << "Refresh button is pushed!";

        auto notes = fetchNotesFromServer();
        emplaceNotes(std::move(notes));
    }

public:
    void init(std::string username, std::string password) {
        currentUser_ = std::move(username);
        currentPassword_ = std::move(password);

        layout.addWidget(&exitButton);
        layout.addWidget(&refreshButton);
        exitButton.setObjectName("pushButton");
        exitButton.setText("Exit");

        refreshButton.setObjectName("refreshButton");
        refreshButton.setText("Refresh");

        window.setLayout(&layout);
        window.setFixedSize(800, 800);

        auto notes = fetchNotesFromServer();
        emplaceNotes(std::move(notes));

        QObject::connect(&exitButton, &QPushButton::clicked, this, &MainWindow::exitButtonPushed);
        QObject::connect(&refreshButton, &QPushButton::clicked, this, &MainWindow::refreshButtonPushed);
    }

    void show() { window.show(); }
};
