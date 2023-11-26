#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);

    QTextEdit *textEdit = new QTextEdit(&window);
    layout->addWidget(textEdit);

    QPushButton *button = new QPushButton("Push me", &window);
    layout->addWidget(button);

    QObject::connect(button, &QPushButton::clicked, [&]() {
        std::cout << textEdit->toPlainText().toStdString() << std::endl;
    });


    window.show();
    return a.exec();
}