#ifndef STOPPERSWINDOW_H
#define STOPPERSWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QDateTime>
#include <qlineedit.h>
#include <qtextedit.h>
#include "portconfig.h"

class MainWindow;

class StoppersWindow : public QMainWindow {
    Q_OBJECT
protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
public:
    explicit StoppersWindow(MainWindow *mainWindow, QWidget *parent = nullptr);

    ~StoppersWindow() override = default;

    void checkForDrop1(int currentDistance);

    void checkForDrop2(int currentDistance);

    void resetStoperCounters();

    void updateStoperDisplay();

    // Metoda do aktualizacji interfejsu w zależności od trybu COM
    void updateUIForComMode(bool useOneCom);

    friend class DebugWindow;
    friend class MainWindow;

private slots:
    void onSensitivityChanged(int sensitivityValue);

    void updateStoper1Time();

    void updateStoper2Time();

private:
    MainWindow *mainWindow;

    void createStoperControls();

    void logDropEvent(int sensorId, int previousDistance, int currentDistance, int difference);

    void startStoper1();

    void stopStoper1();

    void startStoper2();

    void stopStoper2();

    QGroupBox *stoperGroupBox;
    QSlider *sensitivitySlider;
    QLabel *sensitivityLabel;
    QLabel *dropCounter1Label;
    QLabel *dropCounter2Label;
    QCheckBox *enableStoper1CheckBox;
    QCheckBox *enableStoper2CheckBox;
    QTimer *stoperTimer1;
    QTimer *stoperTimer2;
    QLineEdit *timeLabel;
    QLineEdit *timeLabel2;
    QTextEdit *leapsText1;  // For displaying time differences for sensor 1
    QTextEdit *leapsText2;  // For displaying time differences for sensor 2

    // Wskaźnik na box zawierający kontrolki drugiego sensora
    QGroupBox *sensor2GroupBox;

    bool stoper1Running;
    bool stoper2Running;
    int stoper1Time;
    int stoper2Time;
    int dropSensitivity = 50;
    int dropCount1 = 0;
    int dropCount2 = 0;
    int previousDistance1 = 0;
    int previousDistance2 = 0;
    bool stoper1Enabled = true;
    bool stoper2Enabled = true;
    QDateTime lastDropTime1;
    QDateTime lastDropTime2;
    static const int DROP_COOLDOWN_MS = 200;

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
