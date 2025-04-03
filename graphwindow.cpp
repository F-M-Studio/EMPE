#include "graphwindow.h"
#include "./ui_graphwindow.h"
#include "mainwindow.h"
#include "appmenu.h"
#include <QtCharts>
#include <QLineSeries>
#include <QChartView>
#include <QOpenGLWidget>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>

GraphWindow::GraphWindow(MainWindow *mainWindow, QWidget *parent) : QMainWindow(parent), ui(new Ui::GraphWindow),
                                                                    mainWindow(mainWindow) {
    ui->setupUi(this);

    ui->frame->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->frame->setMinimumSize(0, 0);

    // Make sure the frame fills the central widget
    ui->centralwidget->setLayout(new QVBoxLayout());
    ui->centralwidget->layout()->setContentsMargins(0, 0, 0, 10);
    ui->centralwidget->layout()->addWidget(ui->frame);

    auto *appMenu = new AppMenu(this, mainWindow);

    connect(appMenu, &AppMenu::startStopRequested, mainWindow, &MainWindow::handleStartStopButton);
    connect(appMenu, &AppMenu::saveDataRequested, mainWindow, [mainWindow]() {
        mainWindow->saveDataToFile(mainWindow->dataDisplay, "YY(\\d+)T(\\d+)E");
    });

    delete ui->frame->layout();

    // Create the layout first
    auto *clearBtnContainer = new QWidget();
    auto *clearBtnLayout = new QHBoxLayout(clearBtnContainer);
    clearBtnLayout->addStretch();

    // Add start/stop button first (on left)
    startStopBtn = new QPushButton(mainWindow->Reading ? tr("Stop") : tr("Start"));
    startStopBtn->setFixedWidth(80);
    connect(startStopBtn, &QPushButton::clicked, mainWindow, &MainWindow::handleStartStopButton);
    connect(startStopBtn, &QPushButton::clicked, this, [this, mainWindow]() {
        startStopBtn->setText(mainWindow->Reading ? tr("Stop") : tr("Start"));
    });
    clearBtnLayout->addWidget(startStopBtn);

    // Then add clear button (on right)
    clearGraphBtn = new QPushButton(tr("Clear Graph"));
    clearGraphBtn->setFixedWidth(140);
    clearBtnLayout->addWidget(clearGraphBtn);
    clearBtnLayout->addStretch();

    // Connect clear button
    connect(clearGraphBtn, &QPushButton::clicked, this, &GraphWindow::clearGraph);
    series = new QLineSeries();
    series2 = new QLineSeries();
    chart = new QChart();
    const auto chartView = new QChartView(chart);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chart->setAnimationOptions(QChart::NoAnimation);

    axisX = new QValueAxis();
    axisY = new QValueAxis();
    axisX->setTitleText(tr("Time (min:ms)"));
    axisY->setTitleText(tr("Distance"));
    axisX->setRange(mainWindow->distance, mainWindow->distance + 1000);
    axisY->setRange(mainWindow->milliseconds, mainWindow->milliseconds + 100);

    axisX->setMinorTickCount(4);
    axisY->setMinorTickCount(4);
    axisX->setLabelFormat("%d");
    axisY->setLabelFormat("%d");

    chart->addSeries(series);
    chart->addSeries(series2);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    series2->attachAxis(axisX);
    series2->attachAxis(axisY);
    chart->legend()->hide();

    chart->setBackgroundVisible(true);
    chart->setPlotAreaBackgroundVisible(true);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFrameStyle(QFrame::NoFrame);

    // Create recording slider components
    recordingTitleLabel = new QLabel(tr("Recording period [ms]:"));
    recordingSlider = new QSlider(Qt::Horizontal);
    recordingEdit = new QLineEdit("100");
    recordingEdit->setAlignment(Qt::AlignRight);
    recordingEdit->setFixedWidth(50);
    recordingEdit->setValidator(new QIntValidator(10, 1000, this));

    recordingSlider->setRange(10, 1000);
    recordingSlider->setValue(100);

    // Create Y-axis control components
    yAxisToggle = new QCheckBox(tr("Set Y"));
    yAxisTitleLabel = new QLabel(tr("Y axis range:"));
    yAxisTitleLabel->setContentsMargins(10, 0, 0, 0);
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
    autoRemoveToggle = new QCheckBox(tr("Auto-remove points"));
    autoRemoveToggle->setChecked(true);
    pointsLimitLabel = new QLabel(tr("Points limit:"));
    pointsLimitLabel->setContentsMargins(10, 0, 0, 0);
    pointsLimitSlider = new QSlider(Qt::Horizontal);
    pointsLimitEdit = new QLineEdit("100");
    pointsLimitEdit->setAlignment(Qt::AlignRight);
    pointsLimitEdit->setFixedWidth(50);
    pointsLimitEdit->setValidator(new QIntValidator(10, 10000, this));

    pointsLimitSlider->setRange(10, 10000);
    pointsLimitSlider->setValue(100);

    // Create smoothing controls (add after the other controls)
    smoothingToggle = new QCheckBox(tr("Smooth Graph"));
    smoothingToggle->setChecked(false);
    smoothingLevelLabel = new QLabel(tr("Smoothing:"));
    smoothingLevelLabel->setContentsMargins(10, 0, 0, 0);
    smoothingLevelSlider = new QSlider(Qt::Horizontal);
    smoothingLevelEdit = new QLineEdit("5");
    smoothingLevelEdit->setAlignment(Qt::AlignRight);
    smoothingLevelEdit->setFixedWidth(50);
    smoothingLevelEdit->setValidator(new QIntValidator(1, 10, this));

    smoothingLevelSlider->setRange(1, 10);
    smoothingLevelSlider->setValue(5);
    smoothingLevelSlider->setEnabled(false);
    smoothingLevelLabel->setEnabled(false);
    smoothingLevelEdit->setEnabled(false);

    // Create the widget for smoothing controls
    auto *smoothingWidget = new QWidget();
    auto *smoothingLayout = new QHBoxLayout(smoothingWidget);
    // Set the margins and spacing for the layout (change if shitty)
    smoothingLayout->setContentsMargins(11, 2, 11, 2);
    smoothingLayout->setSpacing(4);
    smoothingLayout->addWidget(smoothingToggle);
    smoothingLayout->addWidget(smoothingLevelLabel);
    smoothingLayout->addWidget(smoothingLevelSlider);
    smoothingLayout->addWidget(smoothingLevelEdit);
    smoothingWidget->setLayout(smoothingLayout);

    // Initialize the splineSeries but don't add it to the chart yet
    splineSeries = new QSplineSeries();
    splineSeries2 = new QSplineSeries();

    auto *toggleWidget = new QWidget();
    auto *toggleLayout = new QHBoxLayout(toggleWidget);
    toggleLayout->setContentsMargins(11, 2, 11, 2);
    toggleLayout->setSpacing(4);

    series1Toggle = new QCheckBox(tr("Series 1"));
    series2Toggle = new QCheckBox(tr("Series 2"));
    series1Toggle->setChecked(true);
    series2Toggle->setChecked(true);

    toggleLayout->addWidget(series1Toggle);
    toggleLayout->addWidget(series2Toggle);
    toggleWidget->setLayout(toggleLayout);


    // Create widgets for sliders
    auto *recordingSliderWidget = new QWidget();
    auto *recordingLayout = new QHBoxLayout(recordingSliderWidget);
    // Set the margins and spacing for the layout (change if shitty)
    recordingLayout->setContentsMargins(11, 2, 11, 2);
    recordingLayout->setSpacing(4);


    recordingLayout->addWidget(recordingTitleLabel);
    recordingLayout->addWidget(recordingSlider);
    recordingLayout->addWidget(recordingEdit);
    recordingSliderWidget->setLayout(recordingLayout);

    auto *yAxisSliderWidget = new QWidget();
    auto *yAxisLayout = new QHBoxLayout(yAxisSliderWidget);
    // Set the margins and spacing for the layout (change if shitty)
    yAxisLayout->setContentsMargins(11, 2, 11, 2);
    yAxisLayout->setSpacing(4);

    yAxisLayout->addWidget(yAxisToggle);
    yAxisLayout->addWidget(yAxisTitleLabel);
    yAxisLayout->addWidget(yAxisSlider);
    yAxisLayout->addWidget(yAxisEdit);
    yAxisSliderWidget->setLayout(yAxisLayout);

    auto *pointsLimitWidget = new QWidget();
    auto *pointsLayout = new QHBoxLayout(pointsLimitWidget);
    // Set the margins and spacing for the layout (change if shitty)
    pointsLayout->setContentsMargins(11, 2, 11, 2);
    pointsLayout->setSpacing(4);

    pointsLayout->addWidget(autoRemoveToggle);
    pointsLayout->addWidget(pointsLimitLabel);
    pointsLayout->addWidget(pointsLimitSlider);
    pointsLayout->addWidget(pointsLimitEdit);
    pointsLimitWidget->setLayout(pointsLayout);

    auto *mainLayout = new QVBoxLayout(ui->frame);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(clearBtnContainer);
    mainLayout->addWidget(chartView, 1);
    mainLayout->addWidget(recordingSliderWidget);
    mainLayout->addWidget(yAxisSliderWidget);
    mainLayout->addWidget(pointsLimitWidget);
    ui->frame->setLayout(mainLayout);

    // Enable all rendering hints for high-quality text
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRenderHint(QPainter::TextAntialiasing);
    chartView->setRenderHint(QPainter::SmoothPixmapTransform);
    chartView->setRenderHint(QPainter::LosslessImageRendering);

    // Set better OpenGL format
    QSurfaceFormat format;
    format.setSamples(8); // Increase from 4 to 8 for better anti-aliasing
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapInterval(1);
    format.setAlphaBufferSize(8);

    const auto glWidget = new QOpenGLWidget();
    glWidget->setFormat(format);
    chartView->setViewport(glWidget);


    updateTimer = new QTimer(this);
    updateChartTheme();
    connect(updateTimer, &QTimer::timeout, this, &GraphWindow::updateGraph);

    connect(recordingSlider, &QSlider::valueChanged, this, [this](const int value) {
        recordingEdit->setText(QString::number(value));
        updateTimer->setInterval(value);
    });

    connect(recordingEdit, &QLineEdit::editingFinished, this, [this] {
        const int value = recordingEdit->text().toInt();
        recordingSlider->setValue(value);
        updateTimer->setInterval(value);
    });

    connect(yAxisToggle, &QCheckBox::toggled, this, [this](const bool checked) {
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

    connect(yAxisSlider, &QSlider::valueChanged, this, [this](const int value) {
        yAxisEdit->setText(QString::number(value));
        if (manualYAxisControl) {
            axisY->setRange(-1, value + 1);
        }
    });

    connect(yAxisEdit, &QLineEdit::editingFinished, this, [this]() {
        const int value = yAxisEdit->text().toInt();
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

    connect(pointsLimitSlider, &QSlider::valueChanged, this, [this](const int value) {
        pointsLimitEdit->setText(QString::number(value));
        pointsLimit = value;
    });

    connect(pointsLimitEdit, &QLineEdit::editingFinished, this, [this] {
        const int value = pointsLimitEdit->text().toInt();
        pointsLimitSlider->setValue(value);
        pointsLimit = value;
    });

    mainLayout->addWidget(toggleWidget);

    connect(series1Toggle, &QCheckBox::toggled, this, [this](bool checked) {
        series->setVisible(checked);
        splineSeries->setVisible(checked && useSpline);
    });

    connect(series2Toggle, &QCheckBox::toggled, this, [this](bool checked) {
        series2->setVisible(checked);
        splineSeries2->setVisible(checked && useSpline);
    });

    mainLayout->addWidget(smoothingWidget);

    // Modify the smoothingToggle connection
    connect(smoothingToggle, &QCheckBox::toggled, this, [this](const bool checked) {
        useSpline = checked;
        smoothingLevelSlider->setEnabled(checked);
        smoothingLevelLabel->setEnabled(checked);
        smoothingLevelEdit->setEnabled(checked);

        if (checked) {
            // Apply smoothing to existing points
            applySmoothing();

            // Show the smoothed series
            chart->removeSeries(series);
            chart->addSeries(splineSeries);
            splineSeries->attachAxis(axisX);
            splineSeries->attachAxis(axisY);
        } else {
            // Switch back to original series
            chart->removeSeries(splineSeries);
            chart->addSeries(series);
            series->attachAxis(axisX);
            series->attachAxis(axisY);
        }
    });

    // Update the smoothing level slider connection
    connect(smoothingLevelSlider, &QSlider::valueChanged, this, [this](int value) {
        smoothingLevelEdit->setText(QString::number(value));
        if (useSpline) {
            applySmoothing();
        }
    });

    // Connect smoothing level edit
    connect(smoothingLevelEdit, &QLineEdit::editingFinished, this, [this] {
        const int value = smoothingLevelEdit->text().toInt();
        smoothingLevelSlider->setValue(value);
    });

    timeAxisToggle = new QCheckBox(tr("Use Relative Time"));
    timeAxisToggle->setChecked(true);


    auto *timeAxisWidget = new QWidget();
    auto *timeAxisLayout = new QHBoxLayout(timeAxisWidget);
    timeAxisLayout->setContentsMargins(11, 2, 11, 2);
    timeAxisLayout->setSpacing(4);
    timeAxisLayout->addWidget(timeAxisToggle);
    timeAxisWidget->setLayout(timeAxisLayout);

    mainLayout->addWidget(timeAxisWidget);

    connect(timeAxisToggle, &QCheckBox::toggled, this, [this, mainWindow](bool checked) {
        useAbsoluteTime = !checked; // Toggle is for "Relative Time"
        initialTime = 0; // Reset whenever time mode changes

        if (!useAbsoluteTime && !mainWindow->dataPoints.empty()) {
            // Set initial time to first available data point
            initialTime = mainWindow->dataPoints.first().timeInMilliseconds;
        }

        // Clear existing points to avoid mixed time modes
        series->clear();
        splineSeries->clear();
        series2->clear();
        splineSeries2->clear();
    });

    updateTimer->start(recordingSlider->value());

    QTimer::singleShot(0, this, &GraphWindow::updateChartTheme);
    connect(qApp, &QApplication::paletteChanged, this, &GraphWindow::updateChartTheme);
    updateTimer->start(recordingSlider->value());
}

/* Barely working auto random gen data*/
void GraphWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F5) {
        Gen = !Gen;

        if (Gen) {
            mainWindow->Reading = true;
            // Start generating random data
            auto *genTimer = new QTimer(this);
            connect(genTimer, &QTimer::timeout, this, [this, genTimer]() {
                if (!Gen) {
                    genTimer->stop();
                    genTimer->deleteLater();
                    return;
                }

                // Generate random distance and incrementing time
                static int timeValue = 0;
                timeValue += 10;
                mainWindow->distance = rand() % 50;
                mainWindow->timeInMilliseconds = timeValue;

                // Force an immediate graph update
                updateGraph();
                qDebug() << "GraphWindow::keyPressEvent";
                qDebug() << "timeValue: " << mainWindow->timeInMilliseconds;
                qDebug() << "distance: " << mainWindow->distance;
            });
            genTimer->start(100);
        }
    } else {
        mainWindow->Reading = false;
        QMainWindow::keyPressEvent(event);
    }
}

void GraphWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::PaletteChange) {
        updateChartTheme();
    }
    QMainWindow::changeEvent(event);
}

void GraphWindow::updateChartTheme() {
    if (!chart || !axisX || !axisY || !series || !splineSeries) {
        return; // Guard against null pointers
    }

    QPalette pal = palette();
    bool isDarkMode = pal.color(QPalette::Window).lightness() < 128;

    // Set chart theme based on system theme
    chart->setTheme(isDarkMode ? QChart::ChartThemeDark : QChart::ChartThemeLight);

    QColor bgColor = pal.color(QPalette::Window);
    QColor baseColor = pal.color(QPalette::Base);
    QColor textColor = pal.color(QPalette::Text);

    chart->setBackgroundVisible(true);
    chart->setBackgroundBrush(QBrush(bgColor));
    chart->setBackgroundRoundness(0); // Ensure no rounded corners
    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(QBrush(baseColor));

    // Set text colors
    chart->setTitleBrush(QBrush(textColor));

    // Update axes
    axisX->setLabelsColor(textColor);
    axisY->setLabelsColor(textColor);
    axisX->setTitleBrush(QBrush(textColor));
    axisY->setTitleBrush(QBrush(textColor));
    axisX->setLinePenColor(textColor);
    axisY->setLinePenColor(textColor);

    // Make grid lines very light
    QColor gridColor = textColor;
    gridColor.setAlpha(30); // Reduced from 50
    axisX->setGridLineColor(gridColor);
    axisY->setGridLineColor(gridColor);

    QColor minorGridColor = textColor;
    minorGridColor.setAlpha(15); // Reduced from 25
    axisX->setMinorGridLineColor(minorGridColor);
    axisY->setMinorGridLineColor(minorGridColor);

    // Use high-contrast colors for the series
    QColor primaryColor;
    QColor secondaryColor;

    if (isDarkMode) {
        // Bright colors for dark mode
        primaryColor = QColor(0, 230, 118); // Bright green
        secondaryColor = QColor(255, 128, 0); // Bright orange
    } else {
        // Strong colors for light mode
        primaryColor = QColor(0, 100, 255); // Deep blue
        secondaryColor = QColor(220, 0, 80); // Deep red
    }

    // Create new pens with increased width
    QPen newSeriesPen(primaryColor);
    newSeriesPen.setWidth(3); // Increased from 2
    newSeriesPen.setCapStyle(Qt::RoundCap);
    series->setPen(newSeriesPen);

    QPen newSplinePen(secondaryColor);
    newSplinePen.setWidth(3); // Increased from 2
    newSplinePen.setCapStyle(Qt::RoundCap);
    splineSeries->setPen(newSplinePen);

    QPen newSeries2Pen(secondaryColor);
    newSeries2Pen.setWidth(3);
    if (series2) series2->setPen(newSeries2Pen);
    if (splineSeries2) splineSeries2->setPen(newSeries2Pen);

    // Force a complete redraw
    chart->update();
    chart->scene()->update();
}

void GraphWindow::applySmoothing() const {
    if (!useSpline || series->count() < 2) return;

    const int windowSize = qMin(1 + (smoothingLevelSlider->value() * 10 / 4), 25) | 1; // Ensure odd number
    const int halfWindow = windowSize / 2;
    const auto& points = series->points();
    const int pointCount = points.size();

    QVector<QPointF> smoothedPoints;
    smoothedPoints.reserve(pointCount);

    for (int i = 0; i < pointCount; ++i) {
        double sumX = 0, sumY = 0;
        int count = 0;

        const int start = qMax(0, i - halfWindow);
        const int end = qMin(i + halfWindow, pointCount - 1);

        for (int j = start; j <= end; ++j) {
            sumX += points[j].x();
            sumY += points[j].y();
            count++;
        }

        smoothedPoints.append(QPointF(sumX / count, sumY / count));
    }

    splineSeries->replace(smoothedPoints);
}
void GraphWindow::clearGraph() {
    series->clear();
    splineSeries->clear();
    series2->clear();
    splineSeries2->clear();

    // Reset initial time only in relative mode
    if (!useAbsoluteTime && !mainWindow->dataPoints.empty()) {
        initialTime = mainWindow->dataPoints.first().timeInMilliseconds;
    } else {
        initialTime = 0;
    }
}


GraphWindow::~GraphWindow() {
    delete ui;
}

void GraphWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

void GraphWindow::updateGraph() {
    if (!mainWindow || !mainWindow->Reading) return;

    // Only proceed if we have new data
    static size_t lastDataSize = 0;
    static size_t lastDataSize2 = 0;
    if (mainWindow->dataPoints.size() == lastDataSize &&
        mainWindow->dataPoints2.size() == lastDataSize2) {
        return;
    }
    lastDataSize = mainWindow->dataPoints.size();
    lastDataSize2 = mainWindow->dataPoints2.size();

    // Block signals during updates
    bool oldState = chart->blockSignals(true);

    // Handle first series data
    if (!mainWindow->dataPoints.empty()) {
        const auto& point = mainWindow->dataPoints.back();

        // Set initial time for relative mode
        if (!useAbsoluteTime && initialTime == 0) {
            initialTime = mainWindow->dataPoints.first().timeInMilliseconds;
        }

        // Calculate X value based on time mode
        qreal xValue = useAbsoluteTime
            ? point.timeInMilliseconds / 1000.0  // Absolute time in seconds
            : (point.timeInMilliseconds - initialTime) / 1000.0;  // Relative time from start

        series->append(xValue, point.distance);

        // Auto-remove points handling
        if (autoRemovePoints && series->count() > pointsLimit) {
            series->removePoints(0, series->count() - pointsLimit);
        }
    }

    // Handle second series data
    if (!mainWindow->dataPoints2.empty()) {
        const auto& point = mainWindow->dataPoints2.back();

        // Use same initialTime as first series for consistency
        if (!useAbsoluteTime && initialTime == 0) {
            initialTime = mainWindow->dataPoints2.first().timeInMilliseconds;
        }

        qreal xValue = useAbsoluteTime
            ? point.timeInMilliseconds / 1000.0
            : (point.timeInMilliseconds - initialTime) / 1000.0;

        series2->append(xValue, point.distance);

        if (autoRemovePoints && series2->count() > pointsLimit) {
            series2->removePoints(0, series2->count() - pointsLimit);
        }
    }

    // Apply smoothing if enabled
    if (useSpline) {
        applySmoothing();
    }

    // Adjust axis ranges dynamically
    updateAxisRanges();

    // Restore signal blocking
    chart->blockSignals(oldState);
}

void GraphWindow::updateAxisRanges() {
    if (!manualYAxisControl) {
        double maxY = 0;
        if (series->count() > 0) maxY = series->at(series->count()-1).y();
        if (series2->count() > 0) maxY = qMax(maxY, series2->at(series2->count()-1).y());
        axisY->setRange(0, maxY * 1.1);
    }

    double maxX = useAbsoluteTime ?
        qMax(series->count() > 0 ? series->at(series->count()-1).x() : 0,
             series2->count() > 0 ? series2->at(series2->count()-1).x() : 0) :
        pointsLimit;
    axisX->setRange(0, maxX * 1.1);
}

void GraphWindow::retranslateUi() {
    setWindowTitle(tr("Graph"));
    clearGraphBtn->setText(tr("Clear Graph"));
    startStopBtn->setText(mainWindow->Reading ? tr("Stop") : tr("Start"));
    axisX->setTitleText(tr("Time (min:ms)"));
    axisY->setTitleText(tr("Distance"));
    recordingTitleLabel->setText(tr("Recording period [ms]:"));
    yAxisToggle->setText(tr("Set Y"));
    yAxisTitleLabel->setText(tr("Y axis range:"));
    autoRemoveToggle->setText(tr("Auto-remove points"));
    pointsLimitLabel->setText(tr("Points limit:"));
    smoothingToggle->setText(tr("Smooth Graph"));
    smoothingLevelLabel->setText(tr("Smoothing:"));
    timeAxisToggle->setText(tr("Use Relative Time"));
}
