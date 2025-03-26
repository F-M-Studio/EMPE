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
    void setLanguage(const QString& language) const;

    explicit AppMenu(QMainWindow* window, MainWindow* mainWindow = nullptr);
    ~AppMenu() override = default;

    [[nodiscard]] QMenu* getMainMenu() const { return mainMenu; }
    void retranslateUi() const;
    void updateStartStopAction(bool isReading) const;

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