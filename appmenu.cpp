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



// appmenu.cpp
#include "appmenu.h"
#include "mainwindow.h"

AppMenu::AppMenu(QMainWindow *window, MainWindow *mainWindow) : QObject(window), window(window),
                                                                mainWindow(mainWindow) {
    auto *englishAction = new QAction(tr("English"), window);
    auto *polishAction = new QAction(tr("Polski"), window);
    this->englishAction = englishAction;
    this->polishAction = polishAction;

    mainMenu = new QMenu(tr("☰ Menu"), window);
    window->menuBar()->addMenu(mainMenu);

    portSettingsAction = new QAction(tr("Ustawienia portu"), window);
    graphAction = new QAction(tr("Wykres"), window);
    startMeasurementAction = new QAction(tr("Rozpocznij pomiar"), window);
    saveDataAction = new QAction(tr("Zapisz dane do pliku"), window);
    aboutUsAction = new QAction(tr("O nas"), window);


    mainMenu->addAction(portSettingsAction);
    mainMenu->addAction(graphAction);
    mainMenu->addAction(startMeasurementAction);
    mainMenu->addAction(saveDataAction);
    mainMenu->addSeparator();
    mainMenu->addAction(aboutUsAction);


    connect(portSettingsAction, &QAction::triggered, this, &AppMenu::portSettingsRequested);
    connect(graphAction, &QAction::triggered, this, &AppMenu::graphWindowRequested);
    connect(startMeasurementAction, &QAction::triggered, this, &AppMenu::startStopRequested);
    connect(saveDataAction, &QAction::triggered, this, &AppMenu::saveDataRequested);
    connect(aboutUsAction, &QAction::triggered, this, &AppMenu::aboutUsRequested);


    languageAction = new QAction(tr("Language"), window);
    auto *languageMenu = new QMenu(window);
    auto *languageGroup = new QActionGroup(window);
    languageGroup->setExclusive(true);

    englishAction->setCheckable(true);
    polishAction->setCheckable(true);
    englishAction->setChecked(true);

    languageGroup->addAction(englishAction);
    languageGroup->addAction(polishAction);

    languageMenu->addAction(englishAction);
    languageMenu->addAction(polishAction);
    languageAction->setMenu(languageMenu);
    mainMenu->addAction(languageAction);

    connect(englishAction, &QAction::triggered, this, [this]() {
        emit languageChanged("en");
    });

    connect(polishAction, &QAction::triggered, this, [this]() {
        emit languageChanged("pl");
    });
}

void AppMenu::retranslateUi() const {
    if (mainMenu) mainMenu->setTitle(tr("☰ Menu"));
    if (portSettingsAction) portSettingsAction->setText(tr("Ustawienia portu"));
    if (graphAction) graphAction->setText(tr("Wykres"));
    if (startMeasurementAction) startMeasurementAction->setText(tr("Rozpocznij pomiar"));
    if (saveDataAction) saveDataAction->setText(tr("Zapisz dane do pliku"));
    if (aboutUsAction) aboutUsAction->setText(tr("O nas"));
    if (englishAction) englishAction->setText(tr("Angielski"));
    if (polishAction) polishAction->setText(tr("Polski"));
}

void AppMenu::setLanguage(const QString &language) const {
    if (language == "en") {
        englishAction->setChecked(true);
    } else if (language == "pl") {
        polishAction->setChecked(true);
    }
}

void AppMenu::updateStartStopAction(bool isReading) const {
    startMeasurementAction->setText(isReading ? tr("Zatrzymaj pomiar") : tr("Rozpocznij pomiar"));
}