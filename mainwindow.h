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
#include <QTranslator>

// Replace include with forward declaration
class PortSettings;
class AppMenu;
class MovingAverageFilterParallel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    bool Reading{};
    int distance{}, timeInMilliseconds{}, minutes{}, seconds{}, milliseconds{};

    void loadLanguage(const QString &language);

    void handleStartStopButton();

    void openPortSettings() const;

    void openGraphWindow();

    void saveDataToFile();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    // Add these variables for smoothing
    std::vector<double> distanceBuffer;
    MovingAverageFilterParallel *filter = nullptr;
    int filterWindowSize = 5; // Default window size


    AppMenu *appMenu;

    QTimer *validationTimer{};
    bool deviceValidated = false;
    const int VALIDATION_TIMEOUT = 3000;

    QTranslator *translator = nullptr;
    QAction *languageAction{};

    void retranslateUi();

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
    QLabel *yAxisValueLabel{};

    // Serial port
    QSerialPort *serialPort{};

    QTextEdit *dataDisplay;
    bool isReading;

    void createControls();

    bool startReading();

    void stopReading();

    void parseData(const QString &data);

    QCheckBox *rawDataToggle{};
    bool useRawData = false;
    int lastValidDistance = 0;
};

#endif // MAINWINDOW_H
