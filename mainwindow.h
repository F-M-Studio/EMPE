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

#include "portsettings.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool Reading;
    int distance, timeInMilliseconds, minutes, seconds, milliseconds;
    int distance2, timeInMilliseconds2, minutes2, seconds2, milliseconds2;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void handleStartStopButton();

private:
    struct DataPoint {
        int distance;
        int timeInMilliseconds;
    };
    QVector<DataPoint> dataPoints;
    QVector<DataPoint> dataPoints2;

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QComboBox *portBox;
    QCheckBox *alwaysOnTopCheckbox;

    // Data buffers
    QString dataBuffer1;  // Buffer for COM port 1
    QString dataBuffer2;  // Buffer for COM port 2

    // Menu Bar
    QMenuBar *menuBar;
    QMenu *mainMenu;
    PortSettings *portSettings;
    QAction *portSettingsAction;
    QAction *graphAction;
    QAction *startMeasurementAction;
    QAction *saveDataAction;

    // Buttons
    QPushButton *portSettingsBtn;
    QPushButton *showGraphBtn;
    QPushButton *startStopBtn;
    QPushButton *saveDataBtn;
    QPushButton *saveData2Btn;
    QPushButton *clearGraphBtn;
    QPushButton *showRawDataBtn;  // Button to show/hide raw data

    // Sliders & Controls
    QSlider *yAxisSlider;
    QLineEdit *maxYInput;
    QLineEdit *distanceInput;
    QTimeEdit *timeInput;
    QLineEdit *distanceInput2;
    QTimeEdit *timeInput2;
    QLabel *yAxisValueLabel;

    // Serial ports
    QSerialPort *serialPort;
    QSerialPort *serialPort2;

    QTextEdit *dataDisplay;  // TextEdit to display data from COM port 1
    QTextEdit *dataDisplay2; // TextEdit to display data from COM port 2
    bool isReading;  // Flag to indicate if reading is in progress

    void createMenu();
    void createControls();
    void startReading();
    void stopReading();
    void parseData(const QString &data);
    void parseData2(const QString &data);
    void saveDataToFile(const QTextEdit* display, const QString& regex);
    void processBuffer(QString& buffer, QTextEdit* display, void (MainWindow::*parseFunc)(const QString&));
};

#endif // MAINWINDOW_H