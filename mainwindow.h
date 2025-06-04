// mainwindow.h

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
#include <QTextEdit>
#include <QKeyEvent>
#include <QSerialPort>
#include <QtWidgets>
#include <QDateTime>

#include "portsettings.h"

// Replace include with forward declaration
class PortSettings;
class AppMenu;
class MovingAverageFilterParallel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    bool Reading;
    int distance, timeInMilliseconds, minutes, seconds, milliseconds;
    int distance2, timeInMilliseconds2, minutes2, seconds2, milliseconds2;

    void handleStartStopButton();

    void saveDataToFile(const QTextEdit *display, const QString &regex);

    QTextEdit *dataDisplay;
    QTextEdit *dataDisplay2;

    struct DataPoint {
        int distance;
        int timeInMilliseconds;
    };

    QVector<DataPoint> dataPoints;
    QVector<DataPoint> dataPoints2;

    // Stoper drop detection structure
    struct DropEvent {
        QDateTime timestamp;
        int previousValue;
        int currentValue;
        int dropAmount;
        int sensorNumber; // 1 or 2
    };

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // Existing slots
    void showAboutUsDialog();
    // New stoper slots
    void onSensitivityChanged(int value);
    void resetStoperCounters();
    void saveStoperLogs();

private:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QComboBox *portBox{};
    QCheckBox *alwaysOnTopCheckbox{};

    QString dataBuffer1;
    QString dataBuffer2;
    // Add these variables for smoothing
    std::vector<double> distanceBuffer;
    MovingAverageFilterParallel *filter = nullptr;

    void createMenu();

    void createControls();

    void startReading();

    void stopReading();

    void parseData(const QString &data);

    void parseData2(const QString &data);

    void processBuffer(QString &buffer, QTextEdit *display, void (MainWindow::*parseFunc)(const QString &));

    AppMenu *appMenu;

    QTimer *validationTimer{};
    bool deviceValidated = false;
    const int VALIDATION_TIMEOUT = 3000;

    QTranslator *translator = nullptr;
    QAction *languageAction{};

    void retranslateUi();


    // Menu Bar
    QMenuBar *menuBar{};
    QMenu *mainMenu{};
    PortSettings *portSettings;
    QAction *portSettingsAction{};
    QAction *graphAction{};
    QAction *startMeasurementAction{};
    QAction *saveDataAction{};

    // Buttons
    QPushButton *portSettingsBtn{};
    QPushButton *showGraphBtn{};
    QPushButton *startStopBtn{};
    QPushButton *saveDataBtn{};
    QPushButton *saveData2Btn{};
    QPushButton *clearGraphBtn{};
    QPushButton *showRawDataBtn{};

    // Sliders & Controls
    QSlider *yAxisSlider{};
    QLineEdit *maxYInput{};
    QLineEdit *distanceInput{};
    QLineEdit *distanceInput2{};
    QTimeEdit *timeInput{};
    QTimeEdit *timeInput2{};
    QLabel *yAxisValueLabel{};

    // Serial port
    QSerialPort *serialPort{};
    QSerialPort *serialPort2{};

    bool isReading;

    QCheckBox *rawDataToggle{};
    bool useRawData = false;
    int lastValidDistance = 0;
    QLabel *creatorsNoteLabel{};

    // Existing stopwatch constants
    const int SIGNIFICANT_CHANGE_THRESHOLD = 30;
    const int SMALL_CHANGE_THRESHOLD = 10;

    // Existing stopwatch variables
    QTimer *stopwatchTimer1;
    QTimer *stopwatchTimer2;
    int stopwatchTime1 = 0;
    int stopwatchTime2 = 0;
    bool stopwatchRunning1 = false;
    bool stopwatchRunning2 = false;
    QLabel *stopwatchLabel1;
    QLabel *stopwatchLabel2;
    int lastDistance1 = 0;
    int lastDistance2 = 0;

    // Existing stopwatch methods
    void updateStopwatch1();
    void updateStopwatch2();
    void resetStopwatch1();
    void resetStopwatch2();

    QCheckBox *stopwatchCheck1;
    QCheckBox *stopwatchCheck2;

    // NEW STOPER FEATURE VARIABLES
    // Stoper controls
    QGroupBox *stoperGroupBox;
    QSlider *sensitivitySlider;
    QLabel *sensitivityLabel;
    QLabel *dropCounter1Label;
    QLabel *dropCounter2Label;
    QPushButton *resetStoperBtn;
    QPushButton *saveStoperLogsBtn;
    QCheckBox *enableStoper1CheckBox;
    QCheckBox *enableStoper2CheckBox;

    // Stoper data
    int dropSensitivity = 20; // Default sensitivity threshold
    int dropCount1 = 0;
    int dropCount2 = 0;
    int previousDistance1 = 0;
    int previousDistance2 = 0;
    bool stoper1Enabled = true;
    bool stoper2Enabled = true;

    // Drop event logging
    QVector<DropEvent> dropEvents;

    // Stoper methods
    void createStoperControls();
    void checkForDrop1(int currentDistance);
    void checkForDrop2(int currentDistance);
    void logDropEvent(int sensorNumber, int previousValue, int currentValue, int dropAmount);
    void updateStoperDisplay();
};

#endif // MAINWINDOW_H