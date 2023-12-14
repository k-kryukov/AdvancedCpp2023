#include <QApplication>

#include "MainWindow.hpp"
#include "Window.hpp"
#include "glog/logging.h"

int main(int argc, char *argv[]) {
  google::InitGoogleLogging(argv[0]);
  FLAGS_alsologtostderr = 1;
  QApplication a(argc, argv);

  Window w;

  w.start();

  LOG(INFO) << "Executing...";
  return a.exec();
}
