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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // Istniejące sloty
    void showAboutUsDialog();

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
    int filterWindowSize = 5; // Default window size


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
};

#endif // MAINWINDOW_H
