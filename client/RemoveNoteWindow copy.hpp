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

#include "Service.hpp"

class RemoveNoteWindow : public QObject {
    // Q_OBJECT

    QWidget widget;
    QVBoxLayout layout;
    QLineEdit noteNumberText;
    QPushButton button;

    std::string username_;
    std::string password_;

    Service service;
    // std::hash<std::string> stringHasher;
    // MainWindow* mainWindow;
    // RegisterWindow registerWindow;

public:
    RemoveNoteWindow(std::string username, std::string password)
        : username_{username}, password_{password} {}
    virtual ~RemoveNoteWindow() {}

private slots:
    void pushButton() {
        auto text = noteNumberText.text().toStdString();

        service.removeNote(username_, password_, std::stoi(text));

        widget.hide();
    }

public:
    void init() {
        layout.addWidget(&noteNumberText);
        layout.addWidget(&button);
        noteNumberText.setText("Note number");

        button.setText("Submit");

        widget.setWindowTitle("Remove note");
        widget.setFixedSize(600, 600);
        widget.setLayout(&layout);

        QObject::connect(&button, &QPushButton::clicked, this, &RemoveNoteWindow::pushButton);
    }

    void show() { widget.show(); }
};