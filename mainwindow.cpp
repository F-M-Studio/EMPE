// mainwindow.cpp

#include "mainwindow.h"
#include "portsettings.h"
#include "appmenu.h"
#include "graphwindow.h"
#include "aboutusdialog.h"

#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QRegularExpression>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QApplication>
#include <QPixmap>
#include <QGroupBox>
#include <QGridLayout>
#include <QDateTime>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), isReading(false),
      portSettings(new PortSettings(this)) {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setWindowTitle(tr("EMPE"));

    // Main layout
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    // Menu and controls
    appMenu = new AppMenu(this, this);
    createControls();
    createStoperControls(); // Add stoper controls

    // Data container
    QWidget* dataContainer = new QWidget(this);
    QVBoxLayout* dataLayout = new QVBoxLayout(dataContainer);
    dataLayout->setContentsMargins(0, 0, 0, 0);
    dataLayout->setSpacing(0);

    dataDisplay = new QTextEdit(this);
    dataDisplay->setReadOnly(true);
    dataDisplay->hide();
    dataDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataLayout->addWidget(dataDisplay);

    dataDisplay2 = new QTextEdit(this);
    dataDisplay2->setReadOnly(true);
    dataDisplay2->hide();
    dataDisplay2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataLayout->addWidget(dataDisplay2);

    // Size policy for data container
    dataContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(dataContainer, 1);

    connect(appMenu, &AppMenu::graphWindowRequested, this, [this]() {
        GraphWindow *graphWindow = new GraphWindow(this);
        graphWindow->show();
    });
    connect(appMenu, &AppMenu::portSettingsRequested, this, [this]() {
        PortSettings *portSettings = new PortSettings(this);
        portSettings->show();
    });
    connect(appMenu, &AppMenu::aboutUsRequested, this, &MainWindow::showAboutUsDialog);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap(":/images/FundedByEU.png");
    imageLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    bottomLayout->addWidget(imageLabel);
    mainLayout->addLayout(bottomLayout);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);

    creatorsNoteLabel = new QLabel(tr("Program powstał w ramach projektu Embodying Math&Physics Education 2023-1-PL01-KA210-SCH-000165829"),
                                  this);
    creatorsNoteLabel->setAlignment(Qt::AlignCenter);
    QFont noteFont = creatorsNoteLabel->font();
    noteFont.setPointSize(noteFont.pointSize() - 1);
    noteFont.setItalic(true);
    creatorsNoteLabel->setFont(noteFont);
    creatorsNoteLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    creatorsNoteLabel->setFixedHeight(creatorsNoteLabel->sizeHint().height());
    mainLayout->addWidget(creatorsNoteLabel);
}

MainWindow::~MainWindow() {
    if (isReading) {
        stopReading();
    }
}

void MainWindow::createStoperControls() {
    // Create stoper group box
    stoperGroupBox = new QGroupBox(this);
    QGridLayout *stoperLayout = new QGridLayout(stoperGroupBox);

    // Sensitivity slider
    QLabel *sensLabel = new QLabel(tr("Drop Sensitivity (mm):"), this);
    sensitivitySlider = new QSlider(Qt::Horizontal, this);
    sensitivitySlider->setRange(5, 100);
    sensitivitySlider->setValue(dropSensitivity);
    sensitivitySlider->setTickPosition(QSlider::TicksBelow);
    sensitivitySlider->setTickInterval(5);

    sensitivityLabel = new QLabel(QString::number(dropSensitivity), this);
    sensitivityLabel->setMinimumWidth(5);

    // Control buttons
    resetStoperBtn = new QPushButton(tr("Reset All"), this);

    // Inicjalizacja timera bez przycisku
    stoperTimer = new QTimer(this);
    stoperTimer->setInterval(10); // 10ms interval for smooth countdown
    stoperTime = 60000; // Start with 60 seconds (adjust as needed)
    stoperRunning = false;

    // Add common controls to layout
    stoperLayout->addWidget(sensLabel, 0, 0);
    stoperLayout->addWidget(sensitivitySlider, 0, 1, 1, 2);
    stoperLayout->addWidget(sensitivityLabel, 0, 3);

    // Create sensor 1 frame
    QGroupBox *sensor1GroupBox = new QGroupBox(tr("Sensor 1"), this);
    QVBoxLayout *sensor1Layout = new QVBoxLayout(sensor1GroupBox);

    // Enable checkboxes
    enableStoper1CheckBox = new QCheckBox(tr("Enable Sensor 1"), this);
    enableStoper1CheckBox->setChecked(stoper1Enabled);
    sensor1Layout->addWidget(enableStoper1CheckBox);

    // Drop counter
    dropCounter1Label = new QLabel(tr("Drops: 0"), this);
    dropCounter1Label->setAlignment(Qt::AlignCenter);
    sensor1Layout->addWidget(dropCounter1Label);

    // Add stopwatch
    stopwatchLabel1 = new QLabel(tr("Stoper: 00:00.000"));
    stopwatchLabel1->setAlignment(Qt::AlignCenter);
    stopwatchTimer1 = new QTimer(this);
    stopwatchTimer1->setInterval(10);
    connect(stopwatchTimer1, &QTimer::timeout, this, &MainWindow::updateStopwatch1);
    sensor1Layout->addWidget(stopwatchLabel1);

    // Create sensor 2 frame
    QGroupBox *sensor2GroupBox = new QGroupBox(tr("Sensor 2"), this);
    QVBoxLayout *sensor2Layout = new QVBoxLayout(sensor2GroupBox);

    // Enable checkbox
    enableStoper2CheckBox = new QCheckBox(tr("Enable Sensor 2"), this);
    enableStoper2CheckBox->setChecked(stoper2Enabled);
    sensor2Layout->addWidget(enableStoper2CheckBox);

    // Drop counter
    dropCounter2Label = new QLabel(tr("Drops: 0"), this);
    dropCounter2Label->setAlignment(Qt::AlignCenter);
    sensor2Layout->addWidget(dropCounter2Label);

    // Add stopwatch
    stopwatchLabel2 = new QLabel(tr("Stoper: 00:00.000"));
    stopwatchLabel2->setAlignment(Qt::AlignCenter);
    stopwatchTimer2 = new QTimer(this);
    stopwatchTimer2->setInterval(10);
    connect(stopwatchTimer2, &QTimer::timeout, this, &MainWindow::updateStopwatch2);
    sensor2Layout->addWidget(stopwatchLabel2);

    // Add sensor frames to layout
    stoperLayout->addWidget(sensor1GroupBox, 1, 0, 1, 2);
    stoperLayout->addWidget(sensor2GroupBox, 1, 2, 1, 2);

    // Add reset button only
    stoperLayout->addWidget(resetStoperBtn, 2, 0, 1, 4);

    // Add to main layout
    mainLayout->addWidget(stoperGroupBox);

    // Connect signals
    connect(stoperTimer, &QTimer::timeout, this, &MainWindow::updateStoperTime);
    connect(sensitivitySlider, &QSlider::valueChanged, this, &MainWindow::onSensitivityChanged);
    connect(resetStoperBtn, &QPushButton::clicked, this, &MainWindow::resetStoperCounters);
    connect(enableStoper1CheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        stoper1Enabled = checked;
    });
    connect(enableStoper2CheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        stoper2Enabled = checked;
    });
}

void MainWindow::handleStoperStartStop() {
    if (!stoperRunning) {
        stoperRunning = true;
        startStopStoperBtn->setText(tr("Stop Stoper"));
        stoperTimer->start();
    } else {
        stoperRunning = false;
        startStopStoperBtn->setText(tr("Start Stoper"));
        stoperTimer->stop();
    }
}
void MainWindow::updateStoperTime() {
    if (stoperTime > 0) {
        stoperTime -= 10;
        int mins = stoperTime / 60000;
        int secs = (stoperTime % 60000) / 1000;
        int ms = (stoperTime % 1000) / 10;

        QString timeStr = QString("%1:%2.%3")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'))
            .arg(ms, 2, 10, QChar('0'));

        startStopStoperBtn->setText(tr("Stop Stoper (%1)").arg(timeStr));
    } else {
        stoperTimer->stop();
        stoperRunning = false;
        startStopStoperBtn->setText(tr("Start Stoper"));
        QMessageBox::information(this, tr("Stoper"), tr("Time's up!"));
    }
}
void MainWindow::resetStoperCounters() {
    dropCount1 = 0;
    dropCount2 = 0;
    dropEvents.clear();
    previousDistance1 = 0;
    previousDistance2 = 0;
    lastDropTime1 = QDateTime();
    lastDropTime2 = QDateTime();
    stoperTime = 60000; // Reset to initial time

    // Zatrzymaj główny timer stoper jeśli działa
    if (stoperRunning) {
        stoperTimer->stop();
        stoperRunning = false;
    }

    // Resetuj oba stopery
    resetStopwatch1();
    resetStopwatch2();

    updateStoperDisplay();

    QMessageBox::information(this, tr("Reset Complete"),
                           tr("Drop counters, logs, and stoper have been reset."));
}

void MainWindow::saveStoperLogs() {
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

    // Write all drop events
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

void MainWindow::checkForDrop1(int currentDistance) {
    if (!stoper1Enabled) return;

    // Sprawdzamy, czy jest znacząca różnica odległości (drop)
    int difference = previousDistance1 - currentDistance;

    // Jeśli różnica przekracza próg czułości, wykryto kroplę
    if (difference >= dropSensitivity) {
        // Zwiększ licznik kropli
        dropCount1++;

        // Loguj zdarzenie
        logDropEvent(1, previousDistance1, currentDistance, difference);

        // Przełącz stan stopera (uruchom lub zatrzymaj)
        if (!stopwatchRunning1) {
            // Jeśli stoper nie jest uruchomiony - uruchom go
            stopwatchRunning1 = true;
            stopwatchTimer1->start(10);

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay->append(tr("[%1] Automatyczne uruchomienie stopera 1 po wykryciu kropli")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie uruchomiono stoper 1 po wykryciu kropli";
        } else {
            // Jeśli stoper jest uruchomiony - zatrzymaj go
            stopwatchRunning1 = false;
            stopwatchTimer1->stop();

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay->append(tr("[%1] Automatyczne zatrzymanie stopera 1 po wykryciu kropli")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie zatrzymano stoper 1 po wykryciu kropli";
        }

        // Aktualizuj wyświetlacz
        updateStoperDisplay();

        // Odśwież czas ostatniego wykrycia
        lastDropTime1 = QDateTime::currentDateTime();
    }

    // Zapamiętaj aktualną odległość do następnego porównania
    previousDistance1 = currentDistance;
}

void MainWindow::checkForDrop2(int currentDistance) {
    if (!stoper2Enabled) return;

    // Sprawdzamy, czy jest znacząca różnica odległości (drop)
    int difference = previousDistance2 - currentDistance;

    // Jeśli różnica przekracza próg czułości, wykryto kroplę
    if (difference >= dropSensitivity) {
        // Zwiększ licznik kropli
        dropCount2++;

        // Loguj zdarzenie
        logDropEvent(2, previousDistance2, currentDistance, difference);

        // Przełącz stan stopera (uruchom lub zatrzymaj)
        if (!stopwatchRunning2) {
            // Jeśli stoper nie jest uruchomiony - uruchom go
            stopwatchRunning2 = true;
            stopwatchTimer2->start(10);

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay2->append(tr("[%1] Automatyczne uruchomienie stopera 2 po wykryciu kropli")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie uruchomiono stoper 2 po wykryciu kropli";
        } else {
            // Jeśli stoper jest uruchomiony - zatrzymaj go
            stopwatchRunning2 = false;
            stopwatchTimer2->stop();

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay2->append(tr("[%1] Automatyczne zatrzymanie stopera 2 po wykryciu kropli")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie zatrzymano stoper 2 po wykryciu kropli";
        }

        // Aktualizuj wyświetlacz
        updateStoperDisplay();

        // Odśwież czas ostatniego wykrycia
        lastDropTime2 = QDateTime::currentDateTime();
    }

    // Zapamiętaj aktualną odległość do następnego porównania
    previousDistance2 = currentDistance;
}

void MainWindow::updateStoperDisplay() {
    dropCounter1Label->setText(tr("Drops: %1").arg(dropCount1));
    dropCounter2Label->setText(tr("Drops: %1").arg(dropCount2));
}

void MainWindow::handleStartStopButton() {
    if (isReading) {
        stopReading();
        startStopBtn->setText(tr("START"));
    } else {
        startReading();
        startStopBtn->setText(tr("STOP"));
    }
    isReading = !isReading;
}

void MainWindow::createControls() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    portSettingsBtn = new QPushButton(tr("PORT settings"));
    showGraphBtn = new QPushButton(tr("Show GRAPH"));
    startStopBtn = new QPushButton(tr("START"));
    saveDataBtn = new QPushButton(tr("SAVE data 1"));
    saveData2Btn = new QPushButton(tr("SAVE data 2"));
    clearGraphBtn = new QPushButton(tr("Clear GRAPH"));
    showRawDataBtn = new QPushButton(tr("Show raw data"));

    buttonLayout->addWidget(portSettingsBtn);
    buttonLayout->addWidget(showGraphBtn);
    buttonLayout->addWidget(startStopBtn);
    buttonLayout->addWidget(saveDataBtn);
    buttonLayout->addWidget(saveData2Btn);
    buttonLayout->addWidget(clearGraphBtn);
    buttonLayout->addWidget(showRawDataBtn);

    mainLayout->addLayout(buttonLayout);

    connect(portSettingsBtn, &QPushButton::clicked, this, [this]() {
        portSettings->exec();
    });

    connect(showGraphBtn, &QPushButton::clicked, this, [this]() {
        GraphWindow *graphWindow = new GraphWindow(this);
        graphWindow->show();
    });

    connect(startStopBtn, &QPushButton::clicked, this, &MainWindow::handleStartStopButton);

    connect(saveDataBtn, &QPushButton::clicked, this, [this]() {
        saveDataToFile(dataDisplay, "YY(\\d+)T(\\d+)E");
    });

    connect(saveData2Btn, &QPushButton::clicked, this, [this]() {
        saveDataToFile(dataDisplay2, "YY(\\d+)T(\\d+)E");
    });

    connect(showRawDataBtn, &QPushButton::clicked, this, [this]() {
        bool visible = !dataDisplay->isVisible();
        dataDisplay->setVisible(visible);
        dataDisplay2->setVisible(visible);
        showRawDataBtn->setText(visible ? tr("Hide raw data") : tr("Show raw data"));

        if (!visible) {
            setMinimumSize(0, 0);
            resize(600, 200);
            setMinimumSize(minimumSizeHint());
        }else {
            adjustSize();
        }
    });

    QGridLayout *controlsLayout = new QGridLayout();

    // Sensor 1 distance
    QGroupBox *sensor1Box = new QGroupBox(tr("Sensor 1"));
    QVBoxLayout *sensor1Layout = new QVBoxLayout(sensor1Box);

    QLabel *distanceLabel = new QLabel(tr("Distance:"));
    distanceInput = new QLineEdit("00");
    distanceInput->setReadOnly(true);

    sensor1Layout->addWidget(distanceLabel);
    sensor1Layout->addWidget(distanceInput);

    // Sensor 2 distance
    QGroupBox *sensor2Box = new QGroupBox(tr("Sensor 2"));
    QVBoxLayout *sensor2Layout = new QVBoxLayout(sensor2Box);

    QLabel *distanceLabel2 = new QLabel(tr("Distance:"));
    distanceInput2 = new QLineEdit("00");
    distanceInput2->setReadOnly(true);

    sensor2Layout->addWidget(distanceLabel2);
    sensor2Layout->addWidget(distanceInput2);

    // Dodaj etykietę globalnego czasu
    QGroupBox *timeBox = new QGroupBox(tr("Globalny czas"));
    QVBoxLayout *timeLayout = new QVBoxLayout(timeBox);

    globalTimeLabel = new QLabel("00:00.000");
    globalTimeLabel->setAlignment(Qt::AlignCenter);
    QFont timeFont = globalTimeLabel->font();
    timeFont.setPointSize(timeFont.pointSize() + 2);
    globalTimeLabel->setFont(timeFont);

    timeLayout->addWidget(globalTimeLabel);

    // Dodaj wszystkie boksy do układu
    controlsLayout->addWidget(sensor1Box, 0, 0);
    controlsLayout->addWidget(sensor2Box, 0, 1);
    controlsLayout->addWidget(timeBox, 1, 0, 1, 2);

    // Add sensor boxes to control layout
    controlsLayout->addWidget(sensor1Box, 0, 0);
    controlsLayout->addWidget(sensor2Box, 0, 1);

    mainLayout->addLayout(controlsLayout);

    // Create and add the "Always on Top" checkbox
    alwaysOnTopCheckbox = new QCheckBox(tr("Always on Top"), this);
    mainLayout->addWidget(alwaysOnTopCheckbox);

    connect(alwaysOnTopCheckbox, &QCheckBox::checkStateChanged, this, [this](int state) {
        Qt::WindowFlags flags = windowFlags();
        if (state == Qt::Checked) {
            flags |= Qt::WindowStaysOnTopHint;
        } else {
            flags &= ~Qt::WindowStaysOnTopHint;
        }
        setWindowFlags(flags);
        show();
    });
}

void MainWindow::saveDataToFile(const QTextEdit *display, const QString &regexPattern) {
    QString defaultFileName = QString("EMPE_%1.csv").arg(QDate::currentDate().toString("ddMMyyyy"));
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Data"), defaultFileName,
                                                    tr("CSV Files (*.csv)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    QFile file(fileName);
    if (file.exists()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  tr("File exists"),
                                                                  tr("The file %1 already exists.\nDo you want to replace it?")
                                                                  .arg(QDir::toNativeSeparators(fileName)),
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << tr("Distance,Time (mm:ss),Milliseconds,Raw Time (ms)\n");

    QString rawData = display->toPlainText();
    QStringList lines = rawData.split('\n');
    QRegularExpression regex(regexPattern);
    bool dataWritten = false;

    for (const QString &line: lines) {
        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            QString distance = match.captured(1);
            int timeMs = match.captured(2).toInt();

            int minutes = timeMs / 60000;
            int seconds = (timeMs % 60000) / 1000;
            int milliseconds = timeMs % 1000;

            QString timeFormatted = QString("%1:%2")
                    .arg(minutes, 2, 10, QChar('0'))
                    .arg(seconds, 2, 10, QChar('0'));

            out << QString("%1,%2,%3,%4\n")
                    .arg(distance)
                    .arg(timeFormatted)
                    .arg(milliseconds)
                    .arg(timeMs);

            dataWritten = true;
        }
    }

    file.close();

    if (dataWritten) {
        QMessageBox::information(this, tr("Success"),
                                 tr("Data has been saved to %1")
                                 .arg(QDir::toNativeSeparators(fileName)));
    }
}

void MainWindow::updateStopwatch1() {
    stopwatchTime1 += 10;
    int mins = stopwatchTime1 / 60000;
    int secs = (stopwatchTime1 % 60000) / 1000;
    int ms = stopwatchTime1 % 1000;

    stopwatchLabel1->setText(tr("Stoper: %1:%2.%3")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0')));
}

void MainWindow::updateStopwatch2() {
    stopwatchTime2 += 10;
    int mins = stopwatchTime2 / 60000;
    int secs = (stopwatchTime2 % 60000) / 1000;
    int ms = stopwatchTime2 % 1000;

    stopwatchLabel2->setText(tr("Stoper: %1:%2.%3")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0')));
}

void MainWindow::resetStopwatch1() {
    stopwatchTime1 = 0;
    stopwatchRunning1 = false;
    stopwatchTimer1->stop();
    stopwatchLabel1->setText(tr("Stoper: 00:00.000"));
}

void MainWindow::resetStopwatch2() {
    stopwatchTime2 = 0;
    stopwatchRunning2 = false;
    stopwatchTimer2->stop();
    stopwatchLabel2->setText(tr("Stoper: 00:00.000"));
}

void MainWindow::onSensitivityChanged(int value) {
    dropSensitivity = value;
    sensitivityLabel->setText(QString::number(value));
}

void MainWindow::startReading() {
    resetStopwatch1();
    resetStopwatch2();
    lastDistance1 = 0;
    lastDistance2 = 0;

    // Clear data buffers
    dataBuffer1.clear();
    dataBuffer2.clear();

    // Configure and start Port 1
    QString portName1 = portSettings->getPortName1();
    int baudRate1 = portSettings->getBaudRate1();
    int dataBits1 = portSettings->getDataBits1();
    int stopBits1 = portSettings->getStopBits1();
    int parity1 = portSettings->getParity1();
    int flowControl1 = portSettings->getFlowControl1();

    qDebug() << "Attempting to open port 1:" << portName1
            << "with settings:"
            << "BaudRate:" << baudRate1
            << "DataBits:" << dataBits1
            << "StopBits:" << stopBits1
            << "Parity:" << parity1
            << "FlowControl:" << flowControl1;

    serialPort = new QSerialPort(this);
    serialPort->setPortName(portName1);
    serialPort->setBaudRate(baudRate1);
    serialPort->setDataBits(static_cast<QSerialPort::DataBits>(dataBits1));
    serialPort->setStopBits(static_cast<QSerialPort::StopBits>(stopBits1));
    serialPort->setParity(static_cast<QSerialPort::Parity>(parity1));
    serialPort->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl1));

    if (!serialPort->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open port" << portName1 << "Error:" << serialPort->errorString();
        QMessageBox::warning(this, tr("Error"),
                             tr("Failed to open port %1: %2").arg(portName1, serialPort->errorString()));
        delete serialPort;
        serialPort = nullptr;
        return;
    }

    // Configure and start Port 2
    QString portName2 = portSettings->getPortName2();
    int baudRate2 = portSettings->getBaudRate2();
    int dataBits2 = portSettings->getDataBits2();
    int stopBits2 = portSettings->getStopBits2();
    int parity2 = portSettings->getParity2();
    int flowControl2 = portSettings->getFlowControl2();

    qDebug() << "Attempting to open port 2:" << portName2
            << "with settings:"
            << "BaudRate:" << baudRate2
            << "DataBits:" << dataBits2
            << "StopBits:" << stopBits2
            << "Parity:" << parity2
            << "FlowControl:" << flowControl2;

    serialPort2 = new QSerialPort(this);
    serialPort2->setPortName(portName2);
    serialPort2->setBaudRate(baudRate2);
    serialPort2->setDataBits(static_cast<QSerialPort::DataBits>(dataBits2));
    serialPort2->setStopBits(static_cast<QSerialPort::StopBits>(stopBits2));
    serialPort2->setParity(static_cast<QSerialPort::Parity>(parity2));
    serialPort2->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl2));

    if (!serialPort2->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open port" << portName2 << "Error:" << serialPort2->errorString();
        QMessageBox::warning(this, tr("Error"),
                             tr("Failed to open port %1: %2").arg(portName2, serialPort2->errorString()));

        // Close port 1 if it was opened
        if (serialPort && serialPort->isOpen()) {
            serialPort->close();
            delete serialPort;
            serialPort = nullptr;
        }

        delete serialPort2;
        serialPort2 = nullptr;
        return;
    }

    qDebug() << "Started reading from both ports";
    Reading = true;

    // Connect port 1 signal with buffering
    connect(serialPort, &QSerialPort::readyRead, this, [this]() {
        QByteArray data = serialPort->readAll();
        dataBuffer1.append(QString::fromUtf8(data));
        processBuffer(dataBuffer1, dataDisplay, &MainWindow::parseData);
    });

    // Connect port 2 signal with buffering
    connect(serialPort2, &QSerialPort::readyRead, this, [this]() {
        QByteArray data = serialPort2->readAll();
        dataBuffer2.append(QString::fromUtf8(data));
        processBuffer(dataBuffer2, dataDisplay2, &MainWindow::parseData2);
    });
}

void MainWindow::processBuffer(QString &buffer, QTextEdit *display, void (MainWindow::*parseFunc)(const QString &)) {
    static QRegularExpression completePattern("YY\\d+T\\d+E");

    QString processedData;
    int lastMatchEnd = 0;

    // Find all complete matches in the buffer
    QRegularExpressionMatchIterator matches = completePattern.globalMatch(buffer);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString completeMatch = match.captured(0);

        // Add the complete match to the processed data
        processedData += completeMatch + "\n";

        // Keep track of the end position of the last match
        lastMatchEnd = match.capturedEnd();
    }

    // If we found any complete matches
    if (!processedData.isEmpty()) {
        // Add them to the display
        display->append(processedData);

        // Process data for UI updates (distance, time fields)
        (this->*parseFunc)(processedData); // Call the parsing function

        // Remove processed data, keeping only unprocessed portion
        buffer = buffer.mid(lastMatchEnd);
    }
}

void MainWindow::parseData(const QString &data) {
    static QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(data);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        int newDistance = match.captured(1).toInt();
        if (newDistance > 1000) {
            newDistance = newDistance / 10;
        }
        int newTime = match.captured(2).toInt();

        // Ignoruj nieprawidłowe punkty danych
        if (newDistance == 0 && distance > 0) {
            continue;
        }

        // Aktualizuj bieżące wartości
        distance = newDistance;
        timeInMilliseconds = newTime;

        // Oblicz komponenty czasu
        minutes = timeInMilliseconds / 60000;
        seconds = (timeInMilliseconds % 60000) / 1000;
        milliseconds = timeInMilliseconds % 1000;

        // Zapisz punkt danych
        dataPoints.append({distance, timeInMilliseconds});

        // Aktualizuj UI - tylko pole odległości
        distanceInput->setText(QString::number(distance));

        updateGlobalTimeDisplay(timeInMilliseconds);

        // USUNIĘTO: timeInput->setTime(QTime(0, minutes, seconds, milliseconds));

        // Sprawdzanie kropli (tylko jeśli stoper jest włączony)
        if (stoper1Enabled) {
            checkForDrop1(distance);
        }
    }
}

void MainWindow::parseData2(const QString &data) {
    static QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(data);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        int newDistance = match.captured(1).toInt();
        if (newDistance > 1000) {
            newDistance = newDistance / 10;
        }
        int newTime = match.captured(2).toInt();

        // Ignore invalid data points
        if (newDistance == 0 && distance2 > 0) {
            continue;
        }

        // Update current values
        distance2 = newDistance;
        timeInMilliseconds2 = newTime;

        // Calculate time components
        minutes2 = timeInMilliseconds2 / 60000;
        seconds2 = (timeInMilliseconds2 % 60000) / 1000;
        milliseconds2 = timeInMilliseconds2 % 1000;

        // Store the data point
        dataPoints2.append({distance2, timeInMilliseconds2});

        // Update UI
        distanceInput2->setText(QString::number(distance2));

        updateGlobalTimeDisplay(timeInMilliseconds2);

        // Check for drops (only if stoper is enabled)
        if (stoper2Enabled) {
            checkForDrop2(distance2);
        }
    }
}

void MainWindow::stopReading() {
    Reading = false;

    // Zatrzymaj stopery jeśli działają
    if (stopwatchRunning1) {
        stopwatchRunning1 = false;
        stopwatchTimer1->stop();
    }
    if (stopwatchRunning2) {
        stopwatchRunning2 = false;
        stopwatchTimer2->stop();
    }

    // Close port 1
    if (serialPort && serialPort->isOpen()) {
        disconnect(serialPort, &QSerialPort::readyRead, nullptr, nullptr);
        serialPort->close();
        qDebug() << tr("Stopped reading from port 1");
        delete serialPort;
        serialPort = nullptr;
    }

    // Close port 2
    if (serialPort2 && serialPort2->isOpen()) {
        disconnect(serialPort2, &QSerialPort::readyRead, nullptr, nullptr);
        serialPort2->close();
        qDebug() << tr("Stopped reading from port 2");
        delete serialPort2;
        serialPort2 = nullptr;
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F7) {
        bool visible = !dataDisplay->isVisible();
        dataDisplay->setVisible(visible);
        dataDisplay2->setVisible(visible);
        showRawDataBtn->setText(visible ? tr("Hide raw data") : tr("Show raw data"));
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::showAboutUsDialog() {
    auto *dialog = new AboutUsDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::logDropEvent(int sensorId, int previousDistance, int currentDistance, int difference) {
    QTextEdit *display = (sensorId == 1) ? dataDisplay : dataDisplay2;
    QDateTime currentTime = QDateTime::currentDateTime();

    // Zapisz informację o wykryciu kropli
    display->append(tr("[%1] Wykryto kroplę (sensor %2): Poprzednia odległość: %3, Aktualna: %4, Różnica: %5")
                   .arg(currentTime.toString("hh:mm:ss.zzz"))
                   .arg(sensorId)
                   .arg(previousDistance)
                   .arg(currentDistance)
                   .arg(difference));

    // Przewiń na dół, aby pokazać najnowszy wpis
    QScrollBar *scrollBar = display->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());

    qDebug() << "Wykryto kroplę na sensorze" << sensorId
             << "- Poprzednia:" << previousDistance
             << "Aktualna:" << currentDistance
             << "Różnica:" << difference;
}

void MainWindow::updateGlobalTimeDisplay(int time) {
    // Przetwarzanie czasu do formatu mm:ss.ms
    int mins = time / 60000;
    int secs = (time % 60000) / 1000;
    int ms = time % 1000;

    // Aktualizacja etykiety
    globalTimeLabel->setText(QString("%1:%2.%3")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0')));
}