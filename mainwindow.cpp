// mainwindow.cpp

#include "mainwindow.h"
#include "graphwindow.h"
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
#include <QLineSeries>
#include <QMenu>
#include <QAction>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), portSettings(new PortSettings(this)), isReading(false) {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    createMenu();
    createControls();

    dataDisplay = new QTextEdit(this);
    dataDisplay->setReadOnly(true);
    dataDisplay->hide();
    mainLayout->addWidget(dataDisplay);
}

MainWindow::~MainWindow() {
    if (isReading) {
        stopReading();
    }
}

void MainWindow::createMenu() {
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    mainMenu = new QMenu("☰ Menu", this);
    menuBar->addMenu(mainMenu);

    portSettingsAction = new QAction("Port settings", this);
    graphAction = new QAction("Graph", this);
    startMeasurementAction = new QAction("Start measurement", this);
    saveDataAction = new QAction("Save data to file", this);

    mainMenu->addAction(portSettingsAction);
    mainMenu->addAction(graphAction);
    mainMenu->addAction(startMeasurementAction);
    mainMenu->addAction(saveDataAction);

    connect(portSettingsAction, &QAction::triggered, this, [this]() {
        portSettings->exec();
    });

    connect(graphAction, &QAction::triggered, this, [this]() {
        auto *graphWindow = new GraphWindow(this);
        graphWindow->show();
    });

    connect(startMeasurementAction, &QAction::triggered, this, []() {
        qDebug("Start measurement triggered!");
    });

    connect(saveDataAction, &QAction::triggered, this, []() {
        qDebug("Save data to file triggered!");
    });
}

void MainWindow::createControls() {
    auto *buttonLayout = new QHBoxLayout();

    portSettingsBtn = new QPushButton("PORT settings");
    showGraphBtn = new QPushButton("Show GRAPH");
    stopBtn = new QPushButton("START");
    saveDataBtn = new QPushButton("SAVE data to file");

    buttonLayout->addWidget(portSettingsBtn);
    buttonLayout->addWidget(showGraphBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(saveDataBtn);

    mainLayout->addLayout(buttonLayout);

    connect(portSettingsBtn, &QPushButton::clicked, this, [this]() {
        portSettings->exec();
    });

    connect(showGraphBtn, &QPushButton::clicked, this, [this]() {
        auto *graphWindow = new GraphWindow(this);
        graphWindow->show();
    });

    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::handleStartStopButton);

    connect(saveDataBtn, &QPushButton::clicked, this, [this]() {
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Data"), "",
        tr("CSV Files (*.csv)"));

    // Add .csv extension if not present
    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    if (fileName.isEmpty()) {
        return;
    }

    // Check if file exists
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

    // Write CSV header
    out << "Distance,Time (mm:ss),Milliseconds,Raw Time (ms)\n";

    // Parse and write data from dataDisplay
    QString rawData = dataDisplay->toPlainText();
    QStringList lines = rawData.split('\n');
    QRegularExpression regex("YY(\\d+)T(\\d+)E");

    for (const QString& line : lines) {
        if (QRegularExpressionMatch match = regex.match(line); match.hasMatch()) {
            QString distance = match.captured(1);
            const int timeMs = match.captured(2).toInt();

            // Convert milliseconds to components
            const int minutes = timeMs / 60000;
            const int seconds = (timeMs % 60000) / 1000;
            const int milliseconds = timeMs % 1000;

            // Format time as mm:ss
            QString timeFormatted = QString("%1:%2")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'));

            out << QString("%1,%2,%3,%4\n")
                .arg(distance, timeFormatted)
                .arg(milliseconds)
                .arg(timeMs);
        }
    }

    file.close();

    QMessageBox::information(this, tr("Success"),
        tr("Data has been saved to %1")
        .arg(QDir::toNativeSeparators(fileName)));
    });

    auto *controlsLayout = new QGridLayout();

    auto *distanceLabel = new QLabel("Distance:");
    distanceInput = new QLineEdit("00");
    distanceInput->setReadOnly(true);  // Make the distance label non-editable
    auto *timeLabel = new QLabel("Time:");
    timeInput = new QTimeEdit();
    timeInput->setDisplayFormat("mm:ss.zzz");  // Set the display format to include milliseconds
    timeInput->setReadOnly(true);  // Make the time label non-editable

    controlsLayout->addWidget(distanceLabel, 0, 0);
    controlsLayout->addWidget(distanceInput, 0, 1);
    controlsLayout->addWidget(timeLabel, 1, 0);
    controlsLayout->addWidget(timeInput, 1, 1);

    mainLayout->addLayout(controlsLayout);

    // Create and add the "Always on Top" checkbox
    alwaysOnTopCheckbox = new QCheckBox("Always on Top", this);
    mainLayout->addWidget(alwaysOnTopCheckbox);

    // Connect the checkbox state change to window flag update

    connect(alwaysOnTopCheckbox, &QCheckBox::checkStateChanged, this, [this](int state) {
        Qt::WindowFlags flags = windowFlags();
        if (state == Qt::Checked) {
            flags |= Qt::WindowStaysOnTopHint;
        } else {
            flags &= ~Qt::WindowStaysOnTopHint;
        }
        setWindowFlags(flags);
        show(); // Need to show the window again after changing flags
    });

    rawDataToggle = new QCheckBox("Get Raw Data", this);
    rawDataToggle->setChecked(false); // Default is smoothed data
    mainLayout->addWidget(rawDataToggle);

    connect(rawDataToggle, &QCheckBox::toggled, this, [this](bool checked) {
        useRawData = checked;
    });
}



void MainWindow::handleStartStopButton() {
    if (isReading) {
        stopReading();
        stopBtn->setText("START");
    } else {
        startReading();
        stopBtn->setText("STOP");
    }
    isReading = !isReading;
}


void MainWindow::startReading() {
    QString portName = portSettings->getPortName();
    int baudRate = portSettings->getBaudRate();
    int dataBits = portSettings->getDataBits();
    int stopBits = portSettings->getStopBits();
    int parity = portSettings->getParity();
    int flowControl = portSettings->getFlowControl();

    qDebug() << "Attempting to open port" << portName
             << "with settings:"
             << "BaudRate:" << baudRate
             << "DataBits:" << dataBits
             << "StopBits:" << stopBits
             << "Parity:" << parity
             << "FlowControl:" << flowControl;

    serialPort = new QSerialPort(this);
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(static_cast<QSerialPort::DataBits>(dataBits));
    serialPort->setStopBits(static_cast<QSerialPort::StopBits>(stopBits));
    serialPort->setParity(static_cast<QSerialPort::Parity>(parity));
    serialPort->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl));

    if (!serialPort->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open port" << portName << "Error:" << serialPort->errorString();
        delete serialPort;
        serialPort = nullptr;
        return;
    }

    qDebug() << "Started reading from port" << portName;



    connect(serialPort, &QSerialPort::readyRead, this, [this]() {
        Reading = true;
        QByteArray data = serialPort->readAll();
        dataDisplay->append(data);
        parseData(QString::fromUtf8(data));
    });
}

void MainWindow::parseData(const QString &data) {
    const QRegularExpression regex("YY(\\d+)T(\\d+)E");

    if (const QRegularExpressionMatch match = regex.match(data); match.hasMatch()) {
        const QString distanceStr = match.captured(1);
        const QString timeStr = match.captured(2);
        const int newDistance = distanceStr.toInt();
        timeInMilliseconds = timeStr.toInt();
        minutes = timeInMilliseconds / 60000;
        seconds = timeInMilliseconds % 60000 / 1000;
        milliseconds = timeInMilliseconds % 1000;

        // Apply smoothing logic if not using raw data
        if (!useRawData) {
            // Only update if it's the first reading or if the change is more than 2 units
            if (lastValidDistance == 0 || abs(newDistance - lastValidDistance) > 2) {
                distance = newDistance;
                lastValidDistance = newDistance;
            }
        } else {
            // Use raw data directly
            distance = newDistance;
            lastValidDistance = newDistance;
        }

        // Store the data point
        dataPoints.append({distance, timeInMilliseconds});

        distanceInput->setText(QString::number(distance));
        timeInput->setTime(QTime(0, minutes, seconds, milliseconds));
    }
}

void MainWindow::stopReading() {
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "Stopped reading from port";
        Reading = false;
        delete serialPort;
        serialPort = nullptr;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F7) {
        dataDisplay->setVisible(!dataDisplay->isVisible());
    } else {
        QMainWindow::keyPressEvent(event);
    }
}