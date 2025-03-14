#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.resize(600, 400);
    w.show();

    return QApplication::exec();
}
