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

    QString username_;
    QString password_;

    Service service;

public:
    CreateNoteWindow(QString username, QString password)
        : username_{username}, password_{password} {}
    virtual ~CreateNoteWindow() {}

private slots:
    void pushButton() {
        widget.hide();

        service.createNote(username_, password_, noteText.text());
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