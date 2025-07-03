#include "stopperswindow.h"
#include "mainwindow.h"

StoppersWindow::StoppersWindow(MainWindow* mainWindow, QWidget *parent)
    : QMainWindow(parent), mainWindow(mainWindow),
    stoperRunning(false),
    stoperTime(0),
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

    stoperTimer = new QTimer(this);
    stoperTimer->setInterval(10);
    connect(stoperTimer, &QTimer::timeout, this, &StoppersWindow::updateStoperTime);
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

    // Time display
    globalTimeLabel = new QLabel("00:00:00", this);
    globalTimeLabel->setAlignment(Qt::AlignCenter);
    QFont timeFont = globalTimeLabel->font();
    timeFont.setPointSize(24);
    globalTimeLabel->setFont(timeFont);

    timeLabel = new QLabel("Sensor 1: 00:00:00", this);
    timeLabel2 = new QLabel("Sensor 2: 00:00:00", this);

    // Buttons
    startStopStoperBtn = new QPushButton(tr("Start"), this);
    resetStoperBtn = new QPushButton(tr("Reset"), this);
    saveStoperLogsBtn = new QPushButton(tr("Save Logs"), this);

    // Checkboxes
    enableStoper1CheckBox = new QCheckBox(tr("Enable Sensor 1"), this);
    enableStoper2CheckBox = new QCheckBox(tr("Enable Sensor 2"), this);
    enableStoper1CheckBox->setChecked(true);
    enableStoper2CheckBox->setChecked(true);

    // Add widgets to layout
    stoperLayout->addWidget(sensitivityContainer);
    stoperLayout->addWidget(dropCounter1Label);
    stoperLayout->addWidget(dropCounter2Label);
    stoperLayout->addWidget(globalTimeLabel);
    stoperLayout->addWidget(timeLabel);
    stoperLayout->addWidget(timeLabel2);
    stoperLayout->addWidget(startStopStoperBtn);
    stoperLayout->addWidget(resetStoperBtn);
    stoperLayout->addWidget(saveStoperLogsBtn);
    stoperLayout->addWidget(enableStoper1CheckBox);
    stoperLayout->addWidget(enableStoper2CheckBox);

    // Connect signals
    connect(startStopStoperBtn, &QPushButton::clicked, this, &StoppersWindow::handleStoperStartStop);
    connect(resetStoperBtn, &QPushButton::clicked, this, &StoppersWindow::resetStoperCounters);
    connect(saveStoperLogsBtn, &QPushButton::clicked, this, &StoppersWindow::saveStoperLogs);
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

void StoppersWindow::handleStoperStartStop() {
    stoperRunning = !stoperRunning;
    if (stoperRunning) {
        startStopStoperBtn->setText(tr("Stop"));
        stoperTimer->start();
    } else {
        startStopStoperBtn->setText(tr("Start"));
        stoperTimer->stop();
    }
}

void StoppersWindow::updateStoperTime() {
    stoperTime += 10;
    updateStoperDisplay();
}

void StoppersWindow::updateStoperDisplay() {
    int hours = stoperTime / 3600000;
    int minutes = (stoperTime % 3600000) / 60000;
    int seconds = (stoperTime % 60000) / 1000;
    int milliseconds = stoperTime % 1000;

    QString timeString = QString("%1:%2:%3.%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(milliseconds / 10, 2, 10, QChar('0'));

    globalTimeLabel->setText(timeString);
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
    stoperTime = 0;

    if (stoperRunning) {
        stoperTimer->stop();
        stoperRunning = false;
    }

    updateStoperDisplay();
    startStopStoperBtn->setText(tr("Start"));

    QMessageBox::information(this, tr("Reset Complete"),
                           tr("Drop counters and logs have been reset."));
}

void StoppersWindow::saveStoperLogs() {
    if (dropEvents.isEmpty()) {
        QMessageBox::information(this, tr("No Data"),
                               tr("No drop events to save."));
        return;
    }

    QString defaultFileName = QString("EMPE_DropLogs_%1.csv")
                             .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save Drop Logs"),
                                                  defaultFileName,
                                                  tr("CSV Files (*.csv)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                           tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName),
                           file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << "Timestamp,Sensor,Previous Value (mm),Current Value (mm),Drop Amount (mm),Sensitivity (mm)\n";

    for (const auto &event : dropEvents) {
        out << QString("%1,%2,%3,%4,%5,%6\n")
               .arg(event.timestamp.toString("yyyy-MM-dd HH:mm:ss.zzz"))
               .arg(event.sensorNumber)
               .arg(event.previousValue)
               .arg(event.currentValue)
               .arg(event.dropAmount)
               .arg(dropSensitivity);
    }

    file.close();

    QMessageBox::information(this, tr("Success"),
                           tr("Drop logs saved to %1\nTotal events: %2")
                           .arg(QDir::toNativeSeparators(fileName))
                           .arg(dropEvents.size()));
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
        dropCount2++;
        logDropEvent(2, previousDistance2, currentDistance, difference);
        updateStoperDisplay();
        lastDropTime2 = QDateTime::currentDateTime();
    }

    previousDistance2 = currentDistance;
}
