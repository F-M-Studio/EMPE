/*
* Nazwa Projektu: EMPE
 * Plik: appmenu.cpp
 *
 * Krótki opis pliku: Implementacja klasy menu aplikacji zawierająca funkcjonalności zarządzania interfejsem użytkownika.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 10 Marca 2025
 * Ostatnia Modyfikacja: 27 Czerwaca 2025
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

#include "appmenu.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QSettings>

AppMenu::AppMenu(QMainWindow *window, MainWindow *mainWindow) : QObject(window), window(window),
                                                                mainWindow(mainWindow) {
    mainMenu = new QMenu(tr("☰ Menu"), window);
    window->menuBar()->addMenu(mainMenu);

    // Create actions
    portSettingsAction = new QAction(tr("Port settings"), window);
    graphAction = new QAction(tr("Graph"), window);
    startMeasurementAction = new QAction(tr("Start measurement"), window);
    saveDataAction = new QAction(tr("Save data to file"), window);
    aboutUsAction = new QAction(tr("About us"), window);

    // Connect menu actions to signals
    connect(portSettingsAction, &QAction::triggered, this, &AppMenu::portSettingsRequested);
    connect(graphAction, &QAction::triggered, this, &AppMenu::graphWindowRequested);
    connect(aboutUsAction, &QAction::triggered, this, &AppMenu::aboutUsRequested);

    // Create language menu
    auto *languageMenu = new QMenu(tr("Language"), mainMenu);
    englishAction = new QAction(tr("English"), window);
    polishAction = new QAction(tr("Polish"), window);

    // Connect language actions
    connect(englishAction, &QAction::triggered, this, &AppMenu::switchToEnglish);
    connect(polishAction, &QAction::triggered, this, &AppMenu::switchToPolish);

    // Add actions to language menu
    languageMenu->addAction(englishAction);
    languageMenu->addAction(polishAction);

    // Add all actions to main menu
    mainMenu->addAction(portSettingsAction);
    mainMenu->addAction(graphAction);
    mainMenu->addSeparator();
    mainMenu->addAction(startMeasurementAction);
    mainMenu->addAction(saveDataAction);
    mainMenu->addSeparator();
    mainMenu->addMenu(languageMenu);
    mainMenu->addSeparator();
    mainMenu->addAction(aboutUsAction);

    // Update language checkmarks based on current settings
    QSettings settings("EMPE", "LidarApp");
    QString currentLang = settings.value("language", QString()).toString();
    englishAction->setCheckable(true);
    polishAction->setCheckable(true);
    englishAction->setChecked(currentLang == "en");
    polishAction->setChecked(currentLang == "pl");
}

void AppMenu::switchToEnglish() {
    QSettings settings("EMPE", "LidarApp");
    settings.setValue("language", "en");
    promptRestart();
}

void AppMenu::switchToPolish() {
    QSettings settings("EMPE", "LidarApp");
    settings.setValue("language", "pl");
    promptRestart();
}

void AppMenu::promptRestart() {
    QMessageBox::information(window, tr("Language Changed"),
                           tr("Please restart the application for the language change to take effect."));
}

void AppMenu::updateStartStopAction(bool isReading) const {
    startMeasurementAction->setText(isReading ? tr("Stop measurement") : tr("Start measurement"));
}