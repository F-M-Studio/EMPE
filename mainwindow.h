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
#include <QSerialPort>  // Include QSerialPort header
#include <QtWidgets>

#include "portsettings.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    bool Reading{};
    int distance{}, timeInMilliseconds{}, minutes{}, seconds{}, milliseconds{};

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void handleStartStopButton();
    void openPortSettings() const;
    void openGraphWindow();
    void saveDataToFile();

private:
    struct DataPoint {
        int distance;
        int timeInMilliseconds;
    };
    QVector<DataPoint> dataPoints;

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QComboBox *portBox{};
    QCheckBox *alwaysOnTopCheckbox{};

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
    QPushButton *stopBtn{};
    QPushButton *saveDataBtn{};
    QPushButton *clearGraphBtn{};

    // Sliders & Controls
    QSlider *yAxisSlider{};
    QLineEdit *maxYInput{};
    QLineEdit *distanceInput{};
    QTimeEdit *timeInput{};
    QLabel *yAxisValueLabel{};      // Label to display Y axis scale value

    // Serial port
    QSerialPort *serialPort{};

    QTextEdit *dataDisplay;  // TextEdit to display data from COM port
    bool isReading;  // Flag to indicate if reading is in progress

    void createMenu();
    void createControls();
    void startReading();
    void stopReading();
    void parseData(const QString &data);

    QCheckBox *rawDataToggle{};
    bool useRawData = false;
    int lastValidDistance = 0;
};

#endif // MAINWINDOW_H