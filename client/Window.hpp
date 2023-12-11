#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>

#include <glog/logging.h>
#include "LoginWindow.hpp"

class Window : public QObject {
    MainWindow w;
    LoginWindow loginW;

public:
    Window() : loginW{&w} {}

    void start() {
        LOG(INFO) << "Showing widget...";
        loginW.init();
    }
};