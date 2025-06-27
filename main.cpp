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

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);



    QSettings settings;

    QString translationPath = QCoreApplication::applicationDirPath() + "/translations";

    qDebug() << "Looking for translations in:" << translationPath;



    QString lang = settings.value("language", "en").toString();
    QTranslator translator;
    QString tsFile = translationPath + "/lidar_" + lang;
    qDebug() << "Trying to load translation file:" << tsFile;
    if (!translator.load(tsFile)) {
        qWarning() << "Failed to load translation for" << lang << "from" << tsFile;
        tsFile = translationPath + "/lidar_en";
        if (!translator.load(tsFile)) {
            qWarning() << "Failed to load fallback English translation from" << tsFile;
            qWarning() << "Failed to load fallback translation from" << tsFile;

        }

    }

    a.installTranslator(&translator);
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