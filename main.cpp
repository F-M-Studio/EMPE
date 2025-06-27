/*
* Nazwa Projektu: EMPE
 * Plik: main.cpp
 *
 * Krótki opis pliku: Punkt wejściowy programu, inicjalizacja aplikacji i translatorów.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 4 Marca 2025
 * Ostatnia Modyfikacja: 18 Czerwaca 2025
 *
 * Ten program jest wolnym oprogramowaniem; możesz go rozprowadzać i/lub
 * modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
 * opublikowanej przez Free Software Foundation, w wersji 3 tej Licencji
 * lub (według twojego wyboru) dowolnej późniejszej wersji.
 *
 * Ten program jest rozpowszechniany w nadziei, że będzie użyteczny, ale
 * BEZ ŻADNEJ GWARANCJI; nawet bez domyślnej gwarancji PRZYDATNOŚCI
 * HANDLOWEJ lub PRZYDATNOŚCI DO OKREŚLONEGO CELU. Zobacz Powszechną
 * Licencję Publiczną GNU, aby uzyskać więcej szczegółów.
 *
 * Powinieneś otrzymać kopię Powszechnej Licencji Publicznej GNU wraz z
 * tym programem. Jeśli nie, zobacz <http://www.gnu.org/licenses/>.
*/
#ifdef Q_OS_WIN
    #include <windows.h>
#endif
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QSettings>
#include <QDebug>
#include <QDir>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QSettings settings;
    QString lang = settings.value("language", "pl").toString(); // Domyślny język to polski
    qDebug() << "Wybrany język z ustawień:" << lang;

    // Utworzenie stałego obiektu QTranslator, który będzie żył tak długo jak aplikacja
    static QTranslator translator;

    // Sprawdzamy najpierw w katalogu zasobów (wkompilowane)
    qDebug() << "Próbuję załadować plik z zasobów: :/translations/lidar_" + lang;
    if (translator.load(":/translations/lidar_" + lang)) {
        qDebug() << "Załadowano tłumaczenie z zasobów";
        qDebug() << "Translator jest ważny:" << !translator.isEmpty();
        a.installTranslator(&translator);
    }
    // Następnie sprawdzamy w katalogu aplikacji
    else {
        QString appTranslationPath = QCoreApplication::applicationDirPath() + "/translations";
        qDebug() << "Próbuję załadować plik z:" << appTranslationPath << "/lidar_" + lang;
        if (translator.load("lidar_" + lang, appTranslationPath)) {
            qDebug() << "Załadowano tłumaczenie z:" << appTranslationPath;
            qDebug() << "Translator jest ważny:" << !translator.isEmpty();
            a.installTranslator(&translator);
        }
        // Na koniec sprawdzamy w katalogu projektu
        else {
            QString projTranslationPath = QDir::currentPath() + "/translations";
            qDebug() << "Próbuję załadować plik z:" << projTranslationPath << "/lidar_" + lang;
            if (translator.load("lidar_" + lang, projTranslationPath)) {
                qDebug() << "Załadowano tłumaczenie z:" << projTranslationPath;
                qDebug() << "Translator jest ważny:" << !translator.isEmpty();
                a.installTranslator(&translator);
            } else {
                qWarning() << "Nie udało się załadować pliku tłumaczeń dla języka:" << lang;
                qDebug() << "Sprawdzane lokalizacje:";
                qDebug() << "- :/translations/lidar_" + lang;
                qDebug() << "- " << appTranslationPath << "/lidar_" + lang;
                qDebug() << "- " << projTranslationPath << "/lidar_" + lang;

                // Jeśli nie udało się załadować wybranego języka, próbujemy polski jako domyślny
                if (lang != "pl") {
                    qDebug() << "Próbuję załadować domyślny polski plik tłumaczeń";
                    if (translator.load(":/translations/lidar_pl") ||
                        translator.load("lidar_pl", appTranslationPath) ||
                        translator.load("lidar_pl", projTranslationPath)) {
                        qDebug() << "Załadowano domyślne polskie tłumaczenie";
                        qDebug() << "Translator jest ważny:" << !translator.isEmpty();
                        a.installTranslator(&translator);
                    }
                }
            }
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