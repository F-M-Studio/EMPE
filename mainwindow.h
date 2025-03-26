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
    bool Reading{false};
    int distance{0};
    int timeInMilliseconds{0};
    int minutes{0};
    int seconds{0};
    int milliseconds{0};

    bool isReading2{false};
    int distance2{0};
    int timeInMilliseconds2{0};

    void loadLanguage(const QString &language);
    void handleStartStopButton();
    void openPortSettings() const;
    void openGraphWindow();
    void saveDataToFile();
    void parseData2(const QString &data);


protected:
    void keyPressEvent(QKeyEvent *event) override;

private:

    // Add these variables for smoothing
    std::vector<double> distanceBuffer;
    MovingAverageFilterParallel* filter = nullptr;
    int filterWindowSize = 5;  // Default window size


    AppMenu* appMenu;
    QSerialPort *serialPort{nullptr};
    QSerialPort *serialPort2{nullptr};
    PortSettings *portSettings{nullptr};

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