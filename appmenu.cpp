// appmenu.cpp
#include "appmenu.h"
#include "mainwindow.h"

AppMenu::AppMenu(QMainWindow* window, MainWindow* mainWindow) : QObject(window), window(window), mainWindow(mainWindow) {

    auto* englishAction = new QAction(tr("English"), window);
    auto* polishAction = new QAction(tr("Polish"), window);
    this->englishAction = englishAction;
    this->polishAction = polishAction;

    // Create menu
    mainMenu = new QMenu(tr("☰ Menu"), window);
    window->menuBar()->addMenu(mainMenu);

    // Create actions
    portSettingsAction = new QAction(tr("Port settings"), window);
    graphAction = new QAction(tr("Graph"), window);
    startMeasurementAction = new QAction(tr("Start measurement"), window);
    saveDataAction = new QAction(tr("Save data to file"), window);

    // Add actions to menu
    mainMenu->addAction(portSettingsAction);
    mainMenu->addAction(graphAction);
    mainMenu->addAction(startMeasurementAction);
    mainMenu->addAction(saveDataAction);

    // Connect actions to signals
    connect(portSettingsAction, &QAction::triggered, this, &AppMenu::portSettingsRequested);
    connect(graphAction, &QAction::triggered, this, &AppMenu::graphWindowRequested);
    connect(startMeasurementAction, &QAction::triggered, this, &AppMenu::startStopRequested);
    connect(saveDataAction, &QAction::triggered, this, &AppMenu::saveDataRequested);

    // Language submenu
    languageAction = new QAction(tr("Language"), window);
    auto* languageMenu = new QMenu(window);
    auto* languageGroup = new QActionGroup(window);
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

void AppMenu::retranslateUi() {
    mainMenu->setTitle(tr("☰ Menu"));
    portSettingsAction->setText(tr("Port settings"));
    graphAction->setText(tr("Graph"));
    startMeasurementAction->setText(tr("Start measurement"));
    saveDataAction->setText(tr("Save data to file"));
    languageAction->setText(tr("Language"));

    englishAction->setText(tr("English"));
    polishAction->setText(tr("Polish"));
}

void AppMenu::setLanguage(const QString& language) {
    if (language == "en") {
        englishAction->setChecked(true);
    } else if (language == "pl") {
        polishAction->setChecked(true);
    }
}

void AppMenu::updateStartStopAction(bool isReading) {
    startMeasurementAction->setText(isReading ? tr("Stop measurement") : tr("Start measurement"));
}