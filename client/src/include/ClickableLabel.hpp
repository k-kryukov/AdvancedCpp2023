// #include <QLabel>

// class MainWindow;

// class ClickableLabel : public QLabel
// {
// public:
//     explicit ClickableLabel(const QString &text, QWidget *parent = nullptr)
//         : QLabel(text, parent) {}

//     void setClickCallback(MainWindow* window, std::invocable<> auto f) {
//         QObject::connect(this, &ClickableLabel::mousePressEvent, window, f);
//     }
// };