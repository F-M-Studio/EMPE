// appmenu.h
#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTranslator>
#include <QActionGroup>

class MainWindow;

class AppMenu : public QObject {
    Q_OBJECT

public:
    void setLanguage(const QString& language);

    explicit AppMenu(QMainWindow* window, MainWindow* mainWindow = nullptr);
    ~AppMenu() = default;

    QMenu* getMainMenu() const { return mainMenu; }
    void retranslateUi();
    void updateStartStopAction(bool isReading);

    signals:
        void portSettingsRequested();
    void graphWindowRequested();
    void startStopRequested();
    void saveDataRequested();
    void languageChanged(const QString& language);

private:
    QAction* englishAction;
    QAction* polishAction;

    QMainWindow* window;
    MainWindow* mainWindow;
    QMenu* mainMenu;

    QAction* portSettingsAction;
    QAction* graphAction;
    QAction* startMeasurementAction;
    QAction* saveDataAction;
    QAction* languageAction;
};