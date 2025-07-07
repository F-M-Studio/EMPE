#include "stopperswindow.h"
#include "mainwindow.h"
#include "portconfig.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QFont>
#include <QDebug>
#include <QLineEdit>
#include <QFrame>

StoppersWindow::StoppersWindow(MainWindow *mainWindow, QWidget *parent)
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
      stoper2Time(0) {
    setWindowTitle(tr("Drop Timers"));
    setMinimumSize(500, 200);

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    createStoperControls();

    stoperTimer1 = new QTimer(this);
    stoperTimer1->setInterval(10);
    connect(stoperTimer1, &QTimer::timeout, this, &StoppersWindow::updateStoper1Time);

    stoperTimer2 = new QTimer(this);
    stoperTimer2->setInterval(10);
    connect(stoperTimer2, &QTimer::timeout, this, &StoppersWindow::updateStoper2Time);

    // Aktualizacja interfejsu dla aktualnego trybu COM
    updateUIForComMode(PortConfig::useOneCOM());
}

void StoppersWindow::createStoperControls() {
    // Main container
    auto *mainLayout = qobject_cast<QVBoxLayout *>(centralWidget()->layout());

    // Sensitivity Section
    auto *sensitivityGroupBox = new QGroupBox(tr("Drop Sensitivity"), this);
    auto *sensitivityLayout = new QVBoxLayout(sensitivityGroupBox);
    sensitivityLayout->setSpacing(10);

    sensitivityLabel = new QLabel(tr("Sensitivity: 50 mm"), this);
    sensitivityLabel->setAlignment(Qt::AlignCenter);
    QFont labelFont = sensitivityLabel->font();
    labelFont.setPointSize(10);
    sensitivityLabel->setFont(labelFont);

    sensitivitySlider = new QSlider(Qt::Horizontal, this);
    sensitivitySlider->setRange(1, 100);
    sensitivitySlider->setValue(50);
    sensitivitySlider->setMinimumWidth(300);

    sensitivityLayout->addWidget(sensitivityLabel);
    sensitivityLayout->addWidget(sensitivitySlider);

    // Sensor 1 Section
    auto *sensor1GroupBox = new QGroupBox(tr("Sensor 1"), this);
    auto *sensor1Layout = new QVBoxLayout(sensor1GroupBox);
    sensor1Layout->setSpacing(10);

    enableStoper1CheckBox = new QCheckBox(tr("Enable Sensor 1"), this);
    enableStoper1CheckBox->setChecked(true);

    dropCounter1Label = new QLabel(tr("Drops: 0"), this);
    dropCounter1Label->setAlignment(Qt::AlignCenter);
    dropCounter1Label->setFont(labelFont);

    // Time display as read-only QLineEdit
    timeLabel = new QLineEdit("00:00:00.00", this);
    timeLabel->setReadOnly(true);
    timeLabel->setAlignment(Qt::AlignCenter);
    QFont timeFont = timeLabel->font();
    timeFont.setPointSize(14);

    sensor1Layout->addWidget(enableStoper1CheckBox);
    sensor1Layout->addWidget(dropCounter1Label);
    sensor1Layout->addWidget(timeLabel);

    // Sensor 2 Section
    sensor2GroupBox = new QGroupBox(tr("Sensor 2"), this);
    auto *sensor2Layout = new QVBoxLayout(sensor2GroupBox);
    sensor2Layout->setSpacing(10);

    enableStoper2CheckBox = new QCheckBox(tr("Enable Sensor 2"), this);
    enableStoper2CheckBox->setChecked(true);

    dropCounter2Label = new QLabel(tr("Drops: 0"), this);
    dropCounter2Label->setAlignment(Qt::AlignCenter);
    dropCounter2Label->setFont(labelFont);

    // Time display as read-only QLineEdit
    timeLabel2 = new QLineEdit("00:00:00.00", this);
    timeLabel2->setReadOnly(true);
    timeLabel2->setAlignment(Qt::AlignCenter);
    timeLabel2->setMaximumHeight(35);

    sensor2Layout->addWidget(enableStoper2CheckBox);
    sensor2Layout->addWidget(dropCounter2Label);
    sensor2Layout->addWidget(timeLabel2);

    // Sensors in horizontal layout
    auto *sensorsFrame = new QFrame(this);
    auto *sensorsLayout = new QHBoxLayout(sensorsFrame);
    sensorsLayout->addWidget(sensor1GroupBox);
    sensorsLayout->addWidget(sensor2GroupBox);

    // Add all sections to main layout
    mainLayout->addWidget(sensitivityGroupBox);
    mainLayout->addWidget(sensorsFrame);
    mainLayout->addStretch();

    // Connect signals
    connect(sensitivitySlider, &QSlider::valueChanged, this, &StoppersWindow::onSensitivityChanged);
    connect(enableStoper1CheckBox, &QCheckBox::toggled, this, [this](bool checked) { stoper1Enabled = checked; });
    connect(enableStoper2CheckBox, &QCheckBox::toggled, this, [this](bool checked) { stoper2Enabled = checked; });

}
void StoppersWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    updateUIForComMode(PortConfig::useOneCOM());
}
void StoppersWindow::onSensitivityChanged(int value) {
    dropSensitivity = value;
    sensitivityLabel->setText(tr("Sensitivity: %1 mm").arg(value));
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

    timeLabel->setText(timeString1);
    timeLabel2->setText(timeString2);
    dropCounter1Label->setText(tr("Drops: %1").arg(dropCount1));
    dropCounter2Label->setText(tr("Drops: %1").arg(dropCount2));
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

void StoppersWindow::updateUIForComMode(bool useOneCom) {
    if (sensor2GroupBox) {
        sensor2GroupBox->setVisible(!useOneCom);

        // Reset counters and timers for sensor 2 when switching to one COM
        if (useOneCom) {
            dropCount2 = 0;
            stoper2Time = 0;
            stopStoper2();
            previousDistance2 = 0;
            lastDropTime2 = QDateTime();
            if (timeLabel2) {
                timeLabel2->setText("00:00:00.00");
            }
            if (dropCounter2Label) {
                dropCounter2Label->setText(tr("Drops: 0"));
            }
            if (enableStoper2CheckBox) {
                enableStoper2CheckBox->setChecked(true);
            }
        }

        updateStoperDisplay();
    }
}

void StoppersWindow::checkForDrop1(int currentDistance) {
    if (!stoper1Enabled) return;

    int difference = previousDistance1 - currentDistance;

    if (difference >= dropSensitivity) {
        // Check cooldown
        QDateTime currentTime = QDateTime::currentDateTime();
        if (lastDropTime1.isValid() && lastDropTime1.msecsTo(currentTime) < DROP_COOLDOWN_MS) {
            previousDistance1 = currentDistance;
            return;
        }

        // Toggle stoper1 on each drop from sensor 1
        if (stoper1Running) {
            stopStoper1();
        } else {
            startStoper1();
        }

        dropCount1++;
        logDropEvent(1, previousDistance1, currentDistance, difference);
        updateStoperDisplay();
        lastDropTime1 = currentTime;
    }

    previousDistance1 = currentDistance;
}

void StoppersWindow::checkForDrop2(int currentDistance) {
    if (!stoper2Enabled) return;

    int difference = previousDistance2 - currentDistance;

    if (difference >= dropSensitivity) {
        // Check cooldown
        QDateTime currentTime = QDateTime::currentDateTime();
        if (lastDropTime2.isValid() && lastDropTime2.msecsTo(currentTime) < DROP_COOLDOWN_MS) {
            previousDistance2 = currentDistance;
            return;
        }

        // Toggle stoper2 on each drop from sensor 2
        if (stoper2Running) {
            stopStoper2();
        } else {
            startStoper2();
        }

        dropCount2++;
        logDropEvent(2, previousDistance2, currentDistance, difference);
        updateStoperDisplay();
        lastDropTime2 = currentTime;
    }

    previousDistance2 = currentDistance;
}
