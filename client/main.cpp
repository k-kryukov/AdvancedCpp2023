#include <iostream>

#include <QApplication>
#include <QScreen>
#include <QMainWindow>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

#include "AuthManager.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget window;
    window.setFixedSize(800, 800);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->setAlignment(Qt::AlignCenter);

    QTextEdit *usernameTextArea = new QTextEdit();
    layout->addWidget(usernameTextArea);
    usernameTextArea->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Fixed
    );
    usernameTextArea->setFixedHeight(50);
    usernameTextArea->setAlignment(Qt::AlignCenter);

    QTextEdit *passwordTextArea = new QTextEdit();
    layout->addWidget(passwordTextArea);
    passwordTextArea->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Fixed
    );
    passwordTextArea->setFixedHeight(50);
    passwordTextArea->setAlignment(Qt::AlignCenter);


    QPushButton *loginButton = new QPushButton("One");
    loginButton->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Fixed
    );

    layout->addWidget(loginButton);

    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        AuthManager sender;

        sender.checkCreds(
            usernameTextArea->toPlainText().toStdString(),
            passwordTextArea->toPlainText().toStdString()
        );
    });

    window.show();
    return a.exec();
}