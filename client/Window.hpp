#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QMessageBox>

#include <glog/logging.h>
#include "LoginWindow.hpp"

class Window {
    MainWindow w;
    LoginWindow loginW;

public:
    void start() {

        LOG(INFO) << "Showing widget...";
        loginW.init();
    }

    void tryLogin(std::string login, uint64_t hashedPassword) {
        LOG(INFO) << "Login is " << login << ", pass is " << hashedPassword;
    }
};