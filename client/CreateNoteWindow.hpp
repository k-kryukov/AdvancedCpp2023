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

class CreateNoteWindow : public QObject {
    // Q_OBJECT

    QWidget widget;
    QVBoxLayout layout;
    QLineEdit noteText;
    QPushButton button;

    std::string username_;
    std::string password_;

    Service service;
    // std::hash<std::string> stringHasher;
    // MainWindow* mainWindow;
    // RegisterWindow registerWindow;

public:
    CreateNoteWindow(std::string username, std::string password)
        : username_{username}, password_{password} {}
    virtual ~CreateNoteWindow() {}

private slots:
    void pushButton() {
        auto text = noteText.text().toStdString();

        auto res = service.createNote(username_, password_, text);
        QMessageBox messageBox;
        messageBox.setFixedSize(500,200);
        if (res / 100 != 2)
            messageBox.critical(&widget, "Error", "An error has occured: usually it means that note does not exist");
        else
            messageBox.information(&widget, "OK!", "");

        widget.hide();
    }

public:
    void init() {
        layout.addWidget(&noteText);
        layout.addWidget(&button);

        button.setText("Submit");

        widget.setWindowTitle("New note");
        widget.setFixedSize(600, 600);
        widget.setLayout(&layout);

        QObject::connect(&button, &QPushButton::clicked, this, &CreateNoteWindow::pushButton);
    }

    void show() { widget.show(); }
};