#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QFontDatabase>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Set up translator
    QTranslator translator;
    for (const QStringList uiLanguages = QLocale::system().uiLanguages(); const QString &locale: uiLanguages) {
        if (const QString baseName = "lidar_" + QLocale(locale).name().split('_').first(); translator.load(":/translations/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    QFontDatabase::addApplicationFont(":/fonts/AdwaitaSans-Regular.ttf");

    QFont font;
    font.setFamilies({QString::fromUtf8("Adwaita Sans")});
    QApplication::setFont(font);

    QApplication::setWindowIcon(QIcon(":/icons/EMPE.png"));

    MainWindow w;
    w.resize(600, 200);
    w.show();

    return QApplication::exec();
}