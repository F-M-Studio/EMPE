#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QTimeEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget *centralWidget;

    // Buttons
    QPushButton *portSettingsBtn;
    QPushButton *showGraphBtn;
    QPushButton *stopBtn;
    QPushButton *saveDataBtn;
    QPushButton *clearGraphBtn;

    // Labels and Inputs
    QLabel *distanceLabel;
    QLineEdit *distanceInput;
    QLabel *timeLabel;
    QTimeEdit *timeInput;

    // Sliders
    QLabel *yAxisLabel;
    QSlider *yAxisSlider;
    QLabel *maxYLabel;
    QLineEdit *maxYInput;

    QLabel *recordingLabel;
    QSlider *recordingSlider;

    // Methods
    void setupUI();
    void applyStyles();
};

#endif // MAINWINDOW_H
