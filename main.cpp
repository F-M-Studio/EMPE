#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QApplication::setWindowIcon(QIcon(":/icons/EMPE.png"));

    MainWindow w;
    w.resize(600, 200);
    w.show();

    return QApplication::exec();
}
