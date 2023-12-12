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
#include <QMouseEvent>
#include <glog/logging.h>

#include "Service.hpp"
#include "CreateNoteWindow.hpp"

class MainWindow : public QObject
{
    QPushButton exitButton;
    QPushButton refreshButton;
    QPushButton removeNoteButton;
    QPushButton addNoteButton;
    QVBoxLayout layout;
    QWidget window;
    CreateNoteWindow* createNoteWindow = nullptr;
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

    void removeNoteButtonPushed() {

    }

    void addNoteButtonPushed() {
        createNoteWindow->show();
    }

public:
    void init(std::string username, std::string password) {
        currentUser_ = std::move(username);
        currentPassword_ = std::move(password);

        createNoteWindow = new CreateNoteWindow{currentUser_, currentPassword_};
        createNoteWindow->init();

        layout.addWidget(&exitButton);
        layout.addWidget(&refreshButton);
        layout.addWidget(&removeNoteButton);
        layout.addWidget(&addNoteButton);

        exitButton.setObjectName("pushButton");
        exitButton.setText("Exit");

        refreshButton.setObjectName("refreshButton");
        refreshButton.setText("Refresh");

        removeNoteButton.setObjectName("removeNoteButton");
        removeNoteButton.setText("Remove");

        addNoteButton.setObjectName("addNoteButton");
        addNoteButton.setText("Add");

        window.setLayout(&layout);
        window.setFixedSize(800, 800);

        auto notes = fetchNotesFromServer();
        emplaceNotes(std::move(notes));

        QObject::connect(&exitButton, &QPushButton::clicked, this, &MainWindow::exitButtonPushed);
        QObject::connect(&refreshButton, &QPushButton::clicked, this, &MainWindow::refreshButtonPushed);
        QObject::connect(&removeNoteButton, &QPushButton::clicked, this, &MainWindow::removeNoteButtonPushed);
        QObject::connect(&addNoteButton, &QPushButton::clicked, this, &MainWindow::addNoteButtonPushed);
    }

    void show() { window.show(); }
};
