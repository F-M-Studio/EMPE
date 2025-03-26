// mainwindow.cpp with updated buffering logic

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isReading(false),
      portSettings(new PortSettings(this)) {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    createMenu();
    createControls();

    dataDisplay = new QTextEdit(this);
    dataDisplay->setReadOnly(true);
    dataDisplay->hide();
    mainLayout->addWidget(dataDisplay);

    dataDisplay2 = new QTextEdit(this);
    dataDisplay2->setReadOnly(true);
    dataDisplay2->hide();
    mainLayout->addWidget(dataDisplay2);
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
        GraphWindow *graphWindow = new GraphWindow(this);
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
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    portSettingsBtn = new QPushButton("PORT settings");
    showGraphBtn = new QPushButton("Show GRAPH");
    startStopBtn = new QPushButton("START");
    saveDataBtn = new QPushButton("SAVE data 1");
    saveData2Btn = new QPushButton("SAVE data 2");
    clearGraphBtn = new QPushButton("Clear GRAPH");
    showRawDataBtn = new QPushButton("Show raw data");

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

    // Connect Show Raw Data button
    connect(showRawDataBtn, &QPushButton::clicked, this, [this]() {
        bool visible = !dataDisplay->isVisible();
        dataDisplay->setVisible(visible);
        dataDisplay2->setVisible(visible);
        showRawDataBtn->setText(visible ? "Hide raw data" : "Show raw data");
    });

    QGridLayout *controlsLayout = new QGridLayout();

    QLabel *distanceLabel = new QLabel("Distance 1:");
    distanceInput = new QLineEdit("00");
    distanceInput->setReadOnly(true);
    QLabel *timeLabel = new QLabel("Time 1:");
    timeInput = new QTimeEdit();
    timeInput->setDisplayFormat("mm:ss.zzz");
    timeInput->setReadOnly(true);

    QLabel *distanceLabel2 = new QLabel("Distance 2:");
    distanceInput2 = new QLineEdit("00");
    distanceInput2->setReadOnly(true);
    QLabel *timeLabel2 = new QLabel("Time 2:");
    timeInput2 = new QTimeEdit();
    timeInput2->setDisplayFormat("mm:ss.zzz");
    timeInput2->setReadOnly(true);

    controlsLayout->addWidget(distanceLabel, 0, 0);
    controlsLayout->addWidget(distanceInput, 0, 1);
    controlsLayout->addWidget(timeLabel, 1, 0);
    controlsLayout->addWidget(timeInput, 1, 1);

    controlsLayout->addWidget(distanceLabel2, 0, 2);
    controlsLayout->addWidget(distanceInput2, 0, 3);
    controlsLayout->addWidget(timeLabel2, 1, 2);
    controlsLayout->addWidget(timeInput2, 1, 3);

    mainLayout->addLayout(controlsLayout);

    // Create and add the "Always on Top" checkbox
    alwaysOnTopCheckbox = new QCheckBox("Always on Top", this);
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

void MainWindow::handleStartStopButton() {
    if (isReading) {
        stopReading();
        startStopBtn->setText("START");
    } else {
        startReading();
        startStopBtn->setText("STOP");
    }
    isReading = !isReading;
}

void MainWindow::startReading() {
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

void MainWindow::processBuffer(QString& buffer, QTextEdit* display, void (MainWindow::*parseFunc)(const QString&)) {
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
        (this->*parseFunc)(processedData);  // Call the parsing function

        // Remove processed data, keeping only unprocessed portion
        buffer = buffer.mid(lastMatchEnd);
    }
}

void MainWindow::parseData(const QString &data) {
    // Optimized parsing for first COM port (YYxxxTxxxE format)
    static QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(data);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        // Direct numeric conversion
        distance = match.captured(1).toInt();
        timeInMilliseconds = match.captured(2).toInt();

        // Calculate time components
        minutes = timeInMilliseconds / 60000;
        seconds = (timeInMilliseconds % 60000) / 1000;
        milliseconds = timeInMilliseconds % 1000;

        // Store the data point
        dataPoints.append({distance, timeInMilliseconds});

        // Update UI
        distanceInput->setText(QString::number(distance));
        timeInput->setTime(QTime(0, minutes, seconds, milliseconds));
    }
}

void MainWindow::parseData2(const QString &data) {
    // Optimized parsing for second COM port (using same format as port 1)
    static QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(data);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        // Direct numeric conversion
        distance2 = match.captured(1).toInt();
        timeInMilliseconds2 = match.captured(2).toInt();

        // Calculate time components
        minutes2 = timeInMilliseconds2 / 60000;
        seconds2 = (timeInMilliseconds2 % 60000) / 1000;
        milliseconds2 = timeInMilliseconds2 % 1000;

        // Store the data point
        dataPoints2.append({distance2, timeInMilliseconds2});

        // Update UI
        distanceInput2->setText(QString::number(distance2));
        timeInput2->setTime(QTime(0, minutes2, seconds2, milliseconds2));
    }
}

void MainWindow::stopReading() {
    Reading = false;

    // Close port 1
    if (serialPort && serialPort->isOpen()) {
        disconnect(serialPort, &QSerialPort::readyRead, nullptr, nullptr);
        serialPort->close();
        qDebug() << "Stopped reading from port 1";
        delete serialPort;
        serialPort = nullptr;
    }

    // Close port 2
    if (serialPort2 && serialPort2->isOpen()) {
        disconnect(serialPort2, &QSerialPort::readyRead, nullptr, nullptr);
        serialPort2->close();
        qDebug() << "Stopped reading from port 2";
        delete serialPort2;
        serialPort2 = nullptr;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F7) {
        bool visible = !dataDisplay->isVisible();
        dataDisplay->setVisible(visible);
        dataDisplay2->setVisible(visible);
        showRawDataBtn->setText(visible ? "Hide raw data" : "Show raw data");
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::saveDataToFile(const QTextEdit* display, const QString& regexPattern) {
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

    // Parse and write data from display
    QString rawData = display->toPlainText();
    QStringList lines = rawData.split('\n');
    QRegularExpression regex(regexPattern);

    for (const QString& line : lines) {
        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            QString distance = match.captured(1);
            int timeMs = match.captured(2).toInt();

            // Convert milliseconds to components
            int minutes = timeMs / 60000;
            int seconds = (timeMs % 60000) / 1000;
            int milliseconds = timeMs % 1000;

            // Format time as mm:ss
            QString timeFormatted = QString("%1:%2")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'));

            out << QString("%1,%2,%3,%4\n")
                .arg(distance)
                .arg(timeFormatted)
                .arg(milliseconds)
                .arg(timeMs);
        }
    }

    file.close();

    QMessageBox::information(this, tr("Success"),
        tr("Data has been saved to %1")
        .arg(QDir::toNativeSeparators(fileName)));
}