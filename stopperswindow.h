#ifndef STOPPERSWINDOW_H
#define STOPPERSWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QDateTime>

class MainWindow;

class StoppersWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit StoppersWindow(MainWindow* mainWindow, QWidget *parent = nullptr);
    ~StoppersWindow() override = default;

    void checkForDrop1(int currentDistance);
    void checkForDrop2(int currentDistance);
    void resetStoperCounters();
    void updateStoperDisplay();

    friend class DebugWindow;
    friend class MainWindow;

private slots:
    void onSensitivityChanged(int value);
    void handleStoperStartStop();
    void updateStoperTime();
    void saveStoperLogs();

private:
    MainWindow* mainWindow;
    void createStoperControls();
    void logDropEvent(int sensorId, int previousDistance, int currentDistance, int difference);

    QGroupBox *stoperGroupBox;
    QSlider *sensitivitySlider;
    QLabel *sensitivityLabel;
    QLabel *dropCounter1Label;
    QLabel *dropCounter2Label;
    QPushButton *startStopStoperBtn;
    QPushButton *resetStoperBtn;
    QPushButton *saveStoperLogsBtn;
    QCheckBox *enableStoper1CheckBox;
    QCheckBox *enableStoper2CheckBox;
    QTimer *stoperTimer;
    QLabel *timeLabel;
    QLabel *timeLabel2;
    QLabel *globalTimeLabel;

    bool stoperRunning;
    int stoperTime;
    int dropSensitivity = 50;
    int dropCount1 = 0;
    int dropCount2 = 0;
    int previousDistance1 = 0;
    int previousDistance2 = 0;
    bool stoper1Enabled = true;
    bool stoper2Enabled = true;
    QDateTime lastDropTime1;
    QDateTime lastDropTime2;
    static const int DROP_COOLDOWN_MS = 800;

    struct DropEvent {
        QDateTime timestamp;
        int previousValue;
        int currentValue;
        int dropAmount;
        int sensorNumber;
    };
    QVector<DropEvent> dropEvents;
};

#endif // STOPPERSWINDOW_H
