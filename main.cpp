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
 * Ostatnia Modyfikacja: 18 Czerwca 2025
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

#include <QApplication>
#include <QLocale>
#include <QSettings>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QSettings settings("EMPE", "LidarApp");
    QString selectedLanguage = settings.value("language", QString()).toString();

    QTranslator translator;
    if (!selectedLanguage.isEmpty()) {
        // Load user-selected language
        if (translator.load(":/translations/lidar_" + selectedLanguage)) {
            a.installTranslator(&translator);
        }
    } else {
        // Fallback to system locale
        for (const QString &locale: QLocale::system().uiLanguages()) {
            QString baseName = "lidar_" + QLocale(locale).name().split('_').first();
            if (translator.load(":/translations/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }
    }

    QFontDatabase::addApplicationFont(":/fonts/AdwaitaSans-Regular.ttf");

    MainWindow w;
    w.show();
    return a.exec();
}
