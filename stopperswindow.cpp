#include "stopperswindow.h"
#include "mainwindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QFont>
#include <QDebug>

StoppersWindow::StoppersWindow(MainWindow* mainWindow, QWidget *parent)
    : QMainWindow(parent), mainWindow(mainWindow),
    stoperGroupBox(nullptr),
    sensitivitySlider(nullptr),
    sensitivityLabel(nullptr),
    dropCounter1Label(nullptr),
    dropCounter2Label(nullptr),
    enableStoper1CheckBox(nullptr),
    enableStoper2CheckBox(nullptr),
    stoperTimer1(nullptr),
    stoperTimer2(nullptr),
    timeLabel(nullptr),
    timeLabel2(nullptr),
    stoper1Running(false),
    stoper2Running(false),
    stoper1Time(0),
    stoper2Time(0),
    dropCount1(0),
    dropCount2(0),
    previousDistance1(0),
    previousDistance2(0),
    stoper1Enabled(true),
    stoper2Enabled(true),
    dropSensitivity(50) {

    setWindowTitle(tr("Stoppers"));
    setMinimumSize(400, 300);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto* mainLayout = new QVBoxLayout(centralWidget);

    createStoperControls();

    stoperTimer1 = new QTimer(this);
    stoperTimer1->setInterval(10);
    connect(stoperTimer1, &QTimer::timeout, this, &StoppersWindow::updateStoper1Time);

    stoperTimer2 = new QTimer(this);
    stoperTimer2->setInterval(10);
    connect(stoperTimer2, &QTimer::timeout, this, &StoppersWindow::updateStoper2Time);
}

void StoppersWindow::createStoperControls() {
    stoperGroupBox = new QGroupBox(tr("Stoper Controls"), this);
    auto* stoperLayout = new QVBoxLayout(stoperGroupBox);

    // Sensitivity controls
    auto* sensitivityContainer = new QWidget(this);
    auto* sensitivityLayout = new QHBoxLayout(sensitivityContainer);
    sensitivityLabel = new QLabel(tr("Drop Sensitivity: 50"), this);
    sensitivitySlider = new QSlider(Qt::Horizontal, this);
    sensitivitySlider->setRange(1, 100);
    sensitivitySlider->setValue(50);
    sensitivityLayout->addWidget(sensitivityLabel);
    sensitivityLayout->addWidget(sensitivitySlider);

    // Drop counters
    dropCounter1Label = new QLabel(tr("Sensor 1 Drops: 0"), this);
    dropCounter2Label = new QLabel(tr("Sensor 2 Drops: 0"), this);

    // Individual sensor time displays
    timeLabel = new QLabel("Sensor 1: 00:00:00", this);
    timeLabel2 = new QLabel("Sensor 2: 00:00:00", this);

    // Checkboxes
    enableStoper1CheckBox = new QCheckBox(tr("Enable Sensor 1"), this);
    enableStoper2CheckBox = new QCheckBox(tr("Enable Sensor 2"), this);
    enableStoper1CheckBox->setChecked(true);
    enableStoper2CheckBox->setChecked(true);

    // Add widgets to layout
    stoperLayout->addWidget(sensitivityContainer);
    stoperLayout->addWidget(dropCounter1Label);
    stoperLayout->addWidget(dropCounter2Label);
    stoperLayout->addWidget(timeLabel);
    stoperLayout->addWidget(timeLabel2);
    stoperLayout->addWidget(enableStoper1CheckBox);
    stoperLayout->addWidget(enableStoper2CheckBox);

    // Connect signals
    connect(sensitivitySlider, &QSlider::valueChanged, this, &StoppersWindow::onSensitivityChanged);
    connect(enableStoper1CheckBox, &QCheckBox::toggled, this, [this](bool checked) { stoper1Enabled = checked; });
    connect(enableStoper2CheckBox, &QCheckBox::toggled, this, [this](bool checked) { stoper2Enabled = checked; });

    // Add to central widget
    centralWidget()->layout()->addWidget(stoperGroupBox);
}

void StoppersWindow::onSensitivityChanged(int value) {
    dropSensitivity = value;
    sensitivityLabel->setText(tr("Drop Sensitivity: %1").arg(value));
}

void StoppersWindow::startStoper1() {
    if (!stoper1Running) {
        stoper1Running = true;
        stoperTimer1->start();
    }
}

void StoppersWindow::stopStoper1() {
    if (stoper1Running) {
        stoper1Running = false;
        stoperTimer1->stop();
    }
}

void StoppersWindow::startStoper2() {
    if (!stoper2Running) {
        stoper2Running = true;
        stoperTimer2->start();
    }
}

void StoppersWindow::stopStoper2() {
    if (stoper2Running) {
        stoper2Running = false;
        stoperTimer2->stop();
    }
}

void StoppersWindow::updateStoper1Time() {
    stoper1Time += 10;
    updateStoperDisplay();
}

void StoppersWindow::updateStoper2Time() {
    stoper2Time += 10;
    updateStoperDisplay();
}

void StoppersWindow::updateStoperDisplay() {
    // Update Sensor 1 time
    int hours1 = stoper1Time / 3600000;
    int minutes1 = (stoper1Time % 3600000) / 60000;
    int seconds1 = (stoper1Time % 60000) / 1000;
    int milliseconds1 = stoper1Time % 1000;

    QString timeString1 = QString("%1:%2:%3.%4")
        .arg(hours1, 2, 10, QChar('0'))
        .arg(minutes1, 2, 10, QChar('0'))
        .arg(seconds1, 2, 10, QChar('0'))
        .arg(milliseconds1 / 10, 2, 10, QChar('0'));

    // Update Sensor 2 time
    int hours2 = stoper2Time / 3600000;
    int minutes2 = (stoper2Time % 3600000) / 60000;
    int seconds2 = (stoper2Time % 60000) / 1000;
    int milliseconds2 = stoper2Time % 1000;

    QString timeString2 = QString("%1:%2:%3.%4")
        .arg(hours2, 2, 10, QChar('0'))
        .arg(minutes2, 2, 10, QChar('0'))
        .arg(seconds2, 2, 10, QChar('0'))
        .arg(milliseconds2 / 10, 2, 10, QChar('0'));

    timeLabel->setText(QString("Sensor 1: %1").arg(timeString1));
    timeLabel2->setText(QString("Sensor 2: %1").arg(timeString2));
    dropCounter1Label->setText(tr("Sensor 1 Drops: %1").arg(dropCount1));
    dropCounter2Label->setText(tr("Sensor 2 Drops: %1").arg(dropCount2));
}

void StoppersWindow::resetStoperCounters() {
    dropCount1 = 0;
    dropCount2 = 0;
    dropEvents.clear();
    previousDistance1 = 0;
    previousDistance2 = 0;
    lastDropTime1 = QDateTime();
    lastDropTime2 = QDateTime();
    stoper1Time = 0;
    stoper2Time = 0;

    stopStoper1();
    stopStoper2();
    updateStoperDisplay();
}

void StoppersWindow::logDropEvent(int sensorId, int previousDistance, int currentDistance, int difference) {
    QDateTime currentTime = QDateTime::currentDateTime();

    DropEvent event{
        currentTime,
        previousDistance,
        currentDistance,
        difference,
        sensorId
    };
    dropEvents.append(event);

    qDebug() << "Drop detected on sensor" << sensorId
             << "- Previous:" << previousDistance
             << "Current:" << currentDistance
             << "Difference:" << difference;
}

void StoppersWindow::checkForDrop1(int currentDistance) {
    if (!stoper1Enabled) return;

    int difference = previousDistance1 - currentDistance;

    if (difference >= dropSensitivity) {
        // Toggle stoper1 on each drop from sensor 1
        if (stoper1Running) {
            stopStoper1();
        } else {
            startStoper1();
        }

        dropCount1++;
        logDropEvent(1, previousDistance1, currentDistance, difference);
        updateStoperDisplay();
        lastDropTime1 = QDateTime::currentDateTime();
    }

    previousDistance1 = currentDistance;
}

void StoppersWindow::checkForDrop2(int currentDistance) {
    if (!stoper2Enabled) return;

    int difference = previousDistance2 - currentDistance;

    if (difference >= dropSensitivity) {
        // Toggle stoper2 on each drop from sensor 2
        if (stoper2Running) {
            stopStoper2();
        } else {
            startStoper2();
        }

        dropCount2++;
        logDropEvent(2, previousDistance2, currentDistance, difference);
        updateStoperDisplay();
        lastDropTime2 = QDateTime::currentDateTime();
    }

    previousDistance2 = currentDistance;
}
