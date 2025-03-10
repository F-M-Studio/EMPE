#include "graphwindow.h"
#include "./ui_graphwindow.h"

#include <QtCharts>
#include <QLineSeries>
#include <QChartView>

GraphWindow::GraphWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GraphWindow)
{
    ui->setupUi(this);

    auto *series = new QLineSeries();
    auto chart = new QChart();
    auto *chartView = new QChartView(chart);

    for (int i=0; i<225; i++ )
    {
        series->append(i, i*i);
    }

    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple line chart example");

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setParent(ui->frame);
    chartView->setMinimumSize( ui->frame->size() );
}

GraphWindow::~GraphWindow() {
    delete ui;
}


/*
GraphWindow::~GraphWindow() {
    delete timer;
    delete chart;
    delete series;
    delete axisX;
    delete axisY;
}

void GraphWindow::addDataPoint(qreal x, qreal y) {
    series->append(x, y);
    if (x > axisX->max()) {
        axisX->setMax(x);
    }
    if (y > axisY->max()) {
        axisY->setMax(y);
    }
}

void GraphWindow::updateGraph() {
    qreal distance = std::rand() % 100;  // Generate random distance for debugging
    addDataPoint(time, distance);
    time += 1;
}
*/