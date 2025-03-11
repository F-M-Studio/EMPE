// graphwindow.h
#pragma once

#include "mainwindow.h"
#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class GraphWindow; }
QT_END_NAMESPACE

class GraphWindow : public QMainWindow {
    Q_OBJECT

public:
    GraphWindow(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~GraphWindow();
    void setUpdateInterval(int interval);


private slots:
    void updateGraph();

private:
    Ui::GraphWindow *ui;
    MainWindow *mainWindow;
    QTimer *updateTimer;
    QLineSeries *series;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    const int MAX_POINTS = 100; // Maximum number of points to show

    QSlider *recordingSlider;
    QLabel *recordingLabel;
    QLabel *recordingValueLabel;
};

#endif // GRAPHWINDOW_H