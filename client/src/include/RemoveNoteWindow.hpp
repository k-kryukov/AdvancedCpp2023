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

    QString username_;
    QString password_;

    Service service;
    // MainWindow* mainWindow;
    // RegisterWindow registerWindow;

public:
    RemoveNoteWindow(QString username, QString password)
        : username_{username}, password_{password} {}
    virtual ~RemoveNoteWindow() {}

private slots:
    void pushButton() {
        QMessageBox messageBox;
        messageBox.setFixedSize(500,200);
        widget.hide();

        auto ok = true;
        auto noteNumber = noteNumberText.text().toInt(&ok);
        if (!ok) {
            messageBox.critical(
                &widget,
                "Error",
                "Input is not a number"
            );
            return;
        }

        service.removeNote(username_, password_, noteNumber);
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