#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QTimeEdit>
#include "portsettings.h"  // Include Port Settings Dialog

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;

    // Menu Bar
    QMenuBar *menuBar;
    QMenu *mainMenu;

    QAction *portSettingsAction;
    QAction *graphAction;
    QAction *startMeasurementAction;
    QAction *saveDataAction;

    // Buttons
    QPushButton *portSettingsBtn;
    QPushButton *showGraphBtn;
    QPushButton *stopBtn;
    QPushButton *saveDataBtn;
    QPushButton *clearGraphBtn;

    // Sliders & Controls
    QSlider *yAxisSlider;
    QLineEdit *maxYInput;
    QSlider *recordingSlider;
    QLineEdit *distanceInput;
    QTimeEdit *timeInput;

    void setupUI();
    void createMenu();
    void createControls();
};

#endif // MAINWINDOW_H
