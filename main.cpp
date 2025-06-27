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

    // Dodaję identyfikację wersji Qt
    qDebug() << "Qt version:" << QT_VERSION_STR;
    qDebug() << "---------------";
    qDebug() << "Starting translation system";

    QSettings settings;
    QString lang = settings.value("language", "pl").toString(); // Domyślny język to polski
    qDebug() << "Wybrany język z ustawień:" << lang;

    // Utworzenie stałego obiektu QTranslator, który będzie żył tak długo jak aplikacja
    static QTranslator translator;

    // Sprawdzamy najpierw w katalogu zasobów (wkompilowane)
    qDebug() << "Próbuję załadować plik z zasobów: :/translations/lidar_" + lang;

    // Bardziej szczegółowy debug - sprawdźmy czy pliki zasobów są dostępne
    QDir resourceDir(":/translations");
    if (resourceDir.exists()) {
        qDebug() << "Katalog zasobów istnieje. Zawartość:";
        QStringList files = resourceDir.entryList();
        for (const QString& file : files) {
            qDebug() << "   -" << file;
        }
    } else {
        qDebug() << "UWAGA: Katalog zasobów :/translations nie istnieje!";
    }

    bool loaded = false;
    // Sprawdzamy plik w zasobach
    if (QFile::exists(":/translations/lidar_" + lang + ".qm")) {
        qDebug() << "Plik tłumaczeń istnieje w zasobach:" << ":/translations/lidar_" + lang + ".qm";
        if (translator.load(":/translations/lidar_" + lang)) {
            loaded = true;
            qDebug() << "Załadowano tłumaczenie z zasobów";
            qDebug() << "Translator jest ważny:" << !translator.isEmpty();
        } else {
            qDebug() << "Nie udało się załadować mimo że plik istnieje!";
        }
    } else {
        qDebug() << "Plik tłumaczeń NIE istnieje w zasobach:" << ":/translations/lidar_" + lang + ".qm";
    }

    if (!loaded) {
        // Następnie sprawdzamy w katalogu aplikacji
        QString appTranslationPath = QCoreApplication::applicationDirPath() + "/translations";
        qDebug() << "Próbuję załadować plik z:" << appTranslationPath << "/lidar_" + lang;

        QDir appDir(appTranslationPath);
        if (appDir.exists()) {
            qDebug() << "Katalog aplikacji istnieje. Zawartość:";
            QStringList files = appDir.entryList();
            for (const QString& file : files) {
                qDebug() << "   -" << file;
            }
        } else {
            qDebug() << "UWAGA: Katalog aplikacji" << appTranslationPath << "nie istnieje!";
        }

        if (QFile::exists(appTranslationPath + "/lidar_" + lang + ".qm")) {
            qDebug() << "Plik istnieje:" << appTranslationPath + "/lidar_" + lang + ".qm";
        } else {
            qDebug() << "Plik NIE istnieje:" << appTranslationPath + "/lidar_" + lang + ".qm";
        }

        if (translator.load("lidar_" + lang, appTranslationPath)) {
            loaded = true;
            qDebug() << "Załadowano tłumaczenie z:" << appTranslationPath;
            qDebug() << "Translator jest ważny:" << !translator.isEmpty();
        }
        // Na koniec sprawdzamy w katalogu projektu
        else {
            QString projTranslationPath = QDir::currentPath() + "/translations";
            qDebug() << "Próbuję załadować plik z:" << projTranslationPath << "/lidar_" + lang;

            QDir projDir(projTranslationPath);
            if (projDir.exists()) {
                qDebug() << "Katalog projektu istnieje. Zawartość:";
                QStringList files = projDir.entryList();
                for (const QString& file : files) {
                    qDebug() << "   -" << file;
                }
            } else {
                qDebug() << "UWAGA: Katalog projektu" << projTranslationPath << "nie istnieje!";
            }

            if (QFile::exists(projTranslationPath + "/lidar_" + lang + ".qm")) {
                qDebug() << "Plik istnieje:" << projTranslationPath + "/lidar_" + lang + ".qm";
            } else {
                qDebug() << "Plik NIE istnieje:" << projTranslationPath + "/lidar_" + lang + ".qm";
            }

            if (translator.load("lidar_" + lang, projTranslationPath)) {
                loaded = true;
                qDebug() << "Załadowano tłumaczenie z:" << projTranslationPath;
                qDebug() << "Translator jest ważny:" << !translator.isEmpty();
            } else {
                qWarning() << "Nie udało się załadować pliku tłumaczeń dla języka:" << lang;

                // Próbujemy bezpośrednio załadować plik .qm
                QString absoluteQmPath = projTranslationPath + "/lidar_" + lang + ".qm";
                if (QFile::exists(absoluteQmPath)) {
                    qDebug() << "Próba bezpośredniego załadowania pliku:" << absoluteQmPath;
                    if (translator.load(absoluteQmPath)) {
                        loaded = true;
                        qDebug() << "Załadowano tłumaczenie bezpośrednio z pliku";
                    } else {
                        qDebug() << "Nie udało się załadować pliku mimo podania pełnej ścieżki!";
                    }
                }

                // Jeśli nie udało się załadować wybranego języka, próbujemy polski jako domyślny
                if (!loaded && lang != "pl") {
                    qDebug() << "Próbuję załadować domyślny polski plik tłumaczeń";
                    if (translator.load(":/translations/lidar_pl") ||
                        translator.load("lidar_pl", appTranslationPath) ||
                        translator.load("lidar_pl", projTranslationPath)) {
                        loaded = true;
                        qDebug() << "Załadowano domyślne polskie tłumaczenie";
                        qDebug() << "Translator jest ważny:" << !translator.isEmpty();
                    }
                }
            }
        }
    }

    if (loaded) {
        qDebug() << "Instaluję tłumacza dla języka:" << lang;
        a.installTranslator(&translator);
    } else {
        qDebug() << "UWAGA: Tłumaczenia NIE zostały załadowane!";
    }

    qDebug() << "---------------";

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