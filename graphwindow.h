// graphwindow.h
#pragma once

#include "mainwindow.h"
#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE

namespace Ui {
    class GraphWindow;
}

QT_END_NAMESPACE

class GraphWindow : public QMainWindow {
    Q_OBJECT

public:
    void retranslateUi();

    explicit GraphWindow(MainWindow *mainWindow, QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;

    ~GraphWindow() override;

    [[nodiscard]] QChart *getChart() const { return chart; }

    void changeEvent(QEvent *event) override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateGraph();

    void clearGraph();

private:
    void updateChartTheme();

    AppMenu *appMenu{};

    bool Gen = false;

    Ui::GraphWindow *ui;
    MainWindow *mainWindow;
    QTimer *updateTimer;
    QLineSeries *series;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    const int MAX_POINTS = 100; // Maximum number of points to show

    QSlider *recordingSlider;
    QLabel *recordingLabel{};
    QLabel *recordingValueLabel{};
    QLabel *recordingTitleLabel;
    QLabel *yAxisTitleLabel;
    QLineEdit *recordingEdit;
    QLineEdit *yAxisEdit;

    QPushButton *clearGraphBtn;
    QPushButton *startStopBtn;

    QCheckBox *yAxisToggle;
    QSlider *yAxisSlider;
    QLabel *yAxisLabel{};
    QLabel *yAxisValueLabel{};
    bool manualYAxisControl = false;
    QCheckBox *autoRemoveToggle;
    QSlider *pointsLimitSlider;
    QLineEdit *pointsLimitEdit;
    QLabel *pointsLimitLabel;
    bool autoRemovePoints = true;
    int pointsLimit = 100;


    QSplineSeries *splineSeries;
    QCheckBox *smoothingToggle;
    QSlider *smoothingLevelSlider;
    QLineEdit *smoothingLevelEdit;
    QLabel *smoothingLevelLabel;
    bool useSpline = false;

    void applySmoothing() const;

    QCheckBox *timeAxisToggle;
    bool useAbsoluteTime = false;
    long long initialTime = 0;
};

#endif // GRAPHWINDOW_H
