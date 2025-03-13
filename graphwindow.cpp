#include "graphwindow.h"
#include "./ui_graphwindow.h"
#include "mainwindow.h"
#include <QtCharts>
#include <QLineSeries>
#include <QChartView>
#include <QOpenGLWidget>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>

GraphWindow::GraphWindow(MainWindow *mainWindow, QWidget *parent) : QMainWindow(parent), ui(new Ui::GraphWindow), mainWindow(mainWindow) {
    ui->setupUi(this);

    // Remove fixed size constraints
    ui->frame->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->frame->setMinimumSize(0, 0);

    // Make sure the frame fills the central widget
    ui->centralwidget->setLayout(new QVBoxLayout());
    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->centralwidget->layout()->addWidget(ui->frame);

    delete ui->frame->layout();

    series = new QLineSeries();
    chart = new QChart();
    auto chartView = new QChartView(chart);

    // Make the chart view resize with its container
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QSurfaceFormat format;
    format.setSamples(4);
    format.setSwapInterval(1);

    auto glWidget = new QOpenGLWidget();
    glWidget->setFormat(format);
    chartView->setViewport(glWidget);

    axisX = new QValueAxis();
    axisY = new QValueAxis();
    axisX->setTitleText("Time (min:ms)");
    axisY->setTitleText("Distance");
    axisX->setRange(mainWindow->distance, mainWindow->distance + 1000);
    axisY->setRange(mainWindow->milliseconds, mainWindow->milliseconds + 100);

    axisX->setMinorTickCount(4);
    axisY->setMinorTickCount(4);
    axisX->setLabelFormat("%d");
    axisY->setLabelFormat("%d");

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chart->legend()->hide();

    chart->setBackgroundVisible(true);
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(QBrush(Qt::white));

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFrameStyle(QFrame::NoFrame);

    // Create recording slider components
    recordingTitleLabel = new QLabel("Recording period [ms]:");
    recordingSlider = new QSlider(Qt::Horizontal);
    recordingEdit = new QLineEdit("100");
    recordingEdit->setAlignment(Qt::AlignRight);
    recordingEdit->setFixedWidth(50);
    recordingEdit->setValidator(new QIntValidator(10, 1000, this));

    recordingSlider->setRange(10, 1000);
    recordingSlider->setValue(100);

    // Create Y-axis control components
    yAxisToggle = new QCheckBox("Set Y");
    yAxisTitleLabel = new QLabel("Y axis range:");
    yAxisSlider = new QSlider(Qt::Horizontal);
    yAxisEdit = new QLineEdit("100");
    yAxisEdit->setAlignment(Qt::AlignRight);
    yAxisEdit->setFixedWidth(50);
    yAxisEdit->setValidator(new QIntValidator(0, 10000, this));

    yAxisSlider->setRange(0, 10000);
    yAxisSlider->setValue(100);
    yAxisSlider->setEnabled(false);
    yAxisTitleLabel->setEnabled(false);
    yAxisEdit->setEnabled(false);

    // Create auto-remove points control components
    autoRemoveToggle = new QCheckBox("Auto-remove points");
    autoRemoveToggle->setChecked(true);
    pointsLimitLabel = new QLabel("Points limit:");
    pointsLimitSlider = new QSlider(Qt::Horizontal);
    pointsLimitEdit = new QLineEdit("100");
    pointsLimitEdit->setAlignment(Qt::AlignRight);
    pointsLimitEdit->setFixedWidth(50);
    pointsLimitEdit->setValidator(new QIntValidator(10, 10000, this));

    pointsLimitSlider->setRange(10, 10000);
    pointsLimitSlider->setValue(100);

    // Create widgets for sliders
    QWidget* recordingSliderWidget = new QWidget();
    QHBoxLayout* recordingLayout = new QHBoxLayout(recordingSliderWidget);
    recordingLayout->addWidget(recordingTitleLabel);
    recordingLayout->addWidget(recordingSlider);
    recordingLayout->addWidget(recordingEdit);
    recordingSliderWidget->setLayout(recordingLayout);

    QWidget* yAxisSliderWidget = new QWidget();
    QHBoxLayout* yAxisLayout = new QHBoxLayout(yAxisSliderWidget);
    yAxisLayout->addWidget(yAxisToggle);
    yAxisLayout->addWidget(yAxisTitleLabel);
    yAxisLayout->addWidget(yAxisSlider);
    yAxisLayout->addWidget(yAxisEdit);
    yAxisSliderWidget->setLayout(yAxisLayout);

    QWidget* pointsLimitWidget = new QWidget();
    QHBoxLayout* pointsLayout = new QHBoxLayout(pointsLimitWidget);
    pointsLayout->addWidget(autoRemoveToggle);
    pointsLayout->addWidget(pointsLimitLabel);
    pointsLayout->addWidget(pointsLimitSlider);
    pointsLayout->addWidget(pointsLimitEdit);
    pointsLimitWidget->setLayout(pointsLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(ui->frame);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(chartView, 1);
    mainLayout->addWidget(recordingSliderWidget);
    mainLayout->addWidget(yAxisSliderWidget);
    mainLayout->addWidget(pointsLimitWidget);
    ui->frame->setLayout(mainLayout);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &GraphWindow::updateGraph);

    connect(recordingSlider, &QSlider::valueChanged, this, [this](int value) {
        recordingEdit->setText(QString::number(value));
        updateTimer->setInterval(value);
    });

    connect(recordingEdit, &QLineEdit::editingFinished, this, [this]() {
        int value = recordingEdit->text().toInt();
        recordingSlider->setValue(value);
        updateTimer->setInterval(value);
    });

    connect(yAxisToggle, &QCheckBox::toggled, this, [this](bool checked) {
        yAxisSlider->setEnabled(checked);
        yAxisTitleLabel->setEnabled(checked);
        yAxisEdit->setEnabled(checked);
        if (!checked) {
            manualYAxisControl = false;
        } else {
            manualYAxisControl = true;
            axisY->setRange(-1, yAxisSlider->value() + 1);
        }
    });

    connect(yAxisSlider, &QSlider::valueChanged, this, [this](int value) {
        yAxisEdit->setText(QString::number(value));
        if (manualYAxisControl) {
            axisY->setRange(-1, value + 1);
        }
    });

    connect(yAxisEdit, &QLineEdit::editingFinished, this, [this]() {
        int value = yAxisEdit->text().toInt();
        yAxisSlider->setValue(value);
        if (manualYAxisControl) {
            axisY->setRange(-1, value + 1);
        }
    });

    connect(autoRemoveToggle, &QCheckBox::toggled, this, [this](bool checked) {
        pointsLimitSlider->setEnabled(checked);
        pointsLimitLabel->setEnabled(checked);
        pointsLimitEdit->setEnabled(checked);
        autoRemovePoints = checked;
    });

    connect(pointsLimitSlider, &QSlider::valueChanged, this, [this](int value) {
        pointsLimitEdit->setText(QString::number(value));
        pointsLimit = value;
    });

    connect(pointsLimitEdit, &QLineEdit::editingFinished, this, [this]() {
        int value = pointsLimitEdit->text().toInt();
        pointsLimitSlider->setValue(value);
        pointsLimit = value;
    });

    updateTimer->start(recordingSlider->value());
}

GraphWindow::~GraphWindow() {
    delete ui;
}

void GraphWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (chart) {
        chart->resize(event->size());
    }
}

void GraphWindow::updateGraph() const {
    if (mainWindow->Reading) {
        series->append(mainWindow->timeInMilliseconds, mainWindow->distance);

        if (autoRemovePoints) {
            while (series->count() > pointsLimit) {
                series->remove(0);
            }
        }

        if (series->count() > 0) {
            qreal xMin = series->at(0).x();
            qreal xMax = mainWindow->timeInMilliseconds;
            qreal yMin = mainWindow->distance;
            qreal yMax = mainWindow->distance;

            for (int i = 0; i < series->count(); ++i) {
                yMin = qMin(yMin, series->at(i).y());
                yMax = qMax(yMax, series->at(i).y());
            }

            axisX->setRange(xMin, xMax);
            if (!manualYAxisControl) {
                axisY->setRange(qMax(0.0, yMin) - 1, yMax + 1);
            }
        }
    }
}