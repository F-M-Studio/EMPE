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
 * Ostatnia Modyfikacja: 27 Czerwca 2025
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
#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QFontDatabase>
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

    bool loaded = false;
    QString errorMsg;

    // Wypróbuj różne lokalizacje pliku tłumaczenia w określonej kolejności
    QStringList searchPaths = {
        ":/translations",                              // Z zasobów
        QCoreApplication::applicationDirPath() + "/translations", // Z katalogu aplikacji
        QDir::currentPath() + "/translations"         // Z katalogu projektu
    };

    for (const QString &basePath : searchPaths) {
        QString filePath = basePath + "/lidar_" + lang + ".qm";
        bool fileExists = QFile::exists(filePath);

        qDebug() << "Sprawdzam:" << filePath << (fileExists ? "- istnieje" : "- brak pliku");

        if (fileExists) {
            // Próba załadowania z pełną ścieżką (najbardziej niezawodna metoda)
            if (translator.load(filePath)) {
                qDebug() << "Załadowano tłumaczenie z:" << filePath;
                loaded = true;
                break;
            }

            // Alternatywna próba z separacją nazwy pliku i katalogu
            // (w niektórych wersjach Qt preferowana metoda)
            QString baseName = "lidar_" + lang;
            if (translator.load(baseName, basePath)) {
                qDebug() << "Załadowano tłumaczenie używając separacji:" << basePath << baseName;
                loaded = true;
                break;
            }

            errorMsg += "- Plik " + filePath + " istnieje, ale nie mógł być załadowany\n";
        } else {
            errorMsg += "- Nie znaleziono pliku " + filePath + "\n";
        }
    }

    // Jeśli nie załadowano wybranego języka, próbuj domyślnie polski
    if (!loaded && lang != "pl") {
        qDebug() << "Próba załadowania domyślnego polskiego tłumaczenia";
        for (const QString &basePath : searchPaths) {
            QString filePath = basePath + "/lidar_pl.qm";
            if (QFile::exists(filePath) && translator.load(filePath)) {
                qDebug() << "Załadowano domyślne polskie tłumaczenie z:" << filePath;
                loaded = true;

                // Aktualizuj ustawienia, by odzwierciedlały faktycznie załadowany język
                settings.setValue("language", "pl");
                lang = "pl";
                break;
            }
        }
    }

    if (loaded) {
        qDebug() << "Instaluję tłumacza dla języka:" << lang;
        qDebug() << "Translator jest ważny:" << !translator.isEmpty();
        a.installTranslator(&translator);

        // Opcjonalne powiadomienie dla użytkownika (zakomentuj jeśli nie chcesz pokazywać)
        // QMessageBox::information(nullptr, "Informacja",
        //                          "Załadowano tłumaczenie: " + lang);
    } else {
        qWarning() << "UWAGA: Tłumaczenia NIE zostały załadowane!";
        qWarning() << errorMsg;

        // Pokaż komunikat o błędzie użytkownikowi
        QMessageBox::warning(nullptr, "Błąd tłumaczeń",
                             "Nie udało się załadować pliku tłumaczeń dla języka: " + lang +
                             "\nAplikacja będzie działać bez tłumaczeń.\n\nSzczegóły:\n" + errorMsg);
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