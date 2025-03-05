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
#include "portsettings.h"
#include "lib/serialib.h"  // Include serialib library

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
        void handleStartStopButton();

private:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QComboBox *portBox;
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
    QLabel *recordingValueLabel;  // Label to display recording period value
    QLabel *yAxisValueLabel;      // Label to display Y axis scale value

    serialib serial;  // Serialib object for serial communication
    bool isReading;  // Flag to indicate if reading is in progress

    void setupUI();
    void createMenu();
    void createControls();
    void startReading();
    void stopReading();
};

#endif // MAINWINDOW_H