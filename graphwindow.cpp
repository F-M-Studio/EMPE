#include "graphwindow.h"
#include "./ui_graphwindow.h"
#include "mainwindow.h"
#include <QtCharts>
#include <QLineSeries>
#include <QChartView>
#include <QOpenGLWidget>
#include <QSlider>

GraphWindow::GraphWindow(MainWindow *mainWindow, QWidget *parent) : QMainWindow(parent), ui(new Ui::GraphWindow), mainWindow(mainWindow) {
    ui->setupUi(this);

    // Delete the default horizontal layout
    delete ui->frame->layout();

    // Create series and chart
    series = new QLineSeries();
    chart = new QChart();
    auto chartView = new QChartView(chart);

    // Enable OpenGL with reduced flickering
    QSurfaceFormat format;
    format.setSamples(4);
    format.setSwapInterval(1);

    auto glWidget = new QOpenGLWidget();
    glWidget->setFormat(format);
    chartView->setViewport(glWidget);

    // Setup axes
    axisX = new QValueAxis();
    axisY = new QValueAxis();
    axisX->setTitleText("Time (min:ms)");
    axisY->setTitleText("Distance");
    axisX->setRange(mainWindow->distance, mainWindow->distance + 1000);
    axisY->setRange(mainWindow->milliseconds, mainWindow->milliseconds + 100);

    // Configure axes
    axisX->setMinorTickCount(4);
    axisY->setMinorTickCount(4);
    axisX->setLabelFormat("%d");
    axisY->setLabelFormat("%d");

    // Setup chart
    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->legend()->hide();

    // Set chart visual properties
    chart->setBackgroundVisible(true);
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(QBrush(Qt::white));

    // Setup chart view
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFrameStyle(QFrame::NoFrame);

    // Create recording slider components
    recordingLabel = new QLabel("Recording period [ms]:");
    recordingSlider = new QSlider(Qt::Horizontal);
    recordingValueLabel = new QLabel("100");
    recordingValueLabel->setAlignment(Qt::AlignRight);
    recordingValueLabel->setFixedWidth(50);

    recordingSlider->setRange(10, 1000);
    recordingSlider->setValue(100);

    // Create a widget for slider components
    QWidget* sliderWidget = new QWidget();
    QHBoxLayout* sliderLayout = new QHBoxLayout(sliderWidget);
    sliderLayout->addWidget(recordingLabel);
    sliderLayout->addWidget(recordingSlider);
    sliderLayout->addWidget(recordingValueLabel);
    sliderWidget->setLayout(sliderLayout);

    // Setup vertical layout for frame
    QVBoxLayout* mainLayout = new QVBoxLayout(ui->frame);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(chartView, 1);  // Add stretch factor
    mainLayout->addWidget(sliderWidget);  // Add slider at the bottom
    ui->frame->setLayout(mainLayout);

    // Setup update timer and connect slider
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &GraphWindow::updateGraph);
    connect(recordingSlider, &QSlider::valueChanged, this, [this](int value) {
        recordingValueLabel->setText(QString::number(value));
        updateTimer->setInterval(value);
    });
    updateTimer->start(recordingSlider->value());
}

GraphWindow::~GraphWindow() {
    delete ui;
}

void GraphWindow::updateGraph() {
    if (mainWindow->Reading) {
        // Add new point
        series->append(mainWindow->timeInMilliseconds, mainWindow->distance);

        // Remove old points if we exceed MAX_POINTS
        while (series->count() > MAX_POINTS) {
            series->remove(0);
        }

        if (series->count() > 0) {
            // Calculate ranges
            qreal xMin = series->at(0).x();
            qreal xMax = mainWindow->timeInMilliseconds;
            qreal yMin = mainWindow->distance;
            qreal yMax = mainWindow->distance;

            // Find Y range
            for (int i = 0; i < series->count(); ++i) {
                yMin = qMin(yMin, series->at(i).y());
                yMax = qMax(yMax, series->at(i).y());
            }

            // Update axes with smooth transitions
            axisX->setRange(xMin, xMax);
            axisY->setRange(qMax(0.0, yMin) - 1, yMax + 1);
        }
    }
}