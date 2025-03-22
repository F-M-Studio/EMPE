// mainwindow.cpp

#include "mainwindow.h"
#include "portsettings.h"
#include "appmenu.h"
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

    // Create menu using the new AppMenu class
    appMenu = new AppMenu(this, this);
    connect(appMenu, &AppMenu::portSettingsRequested, this, &MainWindow::openPortSettings);
    connect(appMenu, &AppMenu::graphWindowRequested, this, &MainWindow::openGraphWindow);
    connect(appMenu, &AppMenu::startStopRequested, this, &MainWindow::handleStartStopButton);
    connect(appMenu, &AppMenu::saveDataRequested, this, &MainWindow::saveDataToFile);
    connect(appMenu, &AppMenu::languageChanged, this, &MainWindow::loadLanguage);

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

void MainWindow::createControls() {
    auto *buttonLayout = new QHBoxLayout();

    portSettingsBtn = new QPushButton("Port settings");
    showGraphBtn = new QPushButton("Show Graph");
    stopBtn = new QPushButton("Start");
    saveDataBtn = new QPushButton("Save data to file");

    buttonLayout->addWidget(portSettingsBtn);
    buttonLayout->addWidget(showGraphBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(saveDataBtn);

    mainLayout->addLayout(buttonLayout);

    // Connect buttons to the same functions as menu items
    connect(portSettingsBtn, &QPushButton::clicked, this, &MainWindow::openPortSettings);
    connect(showGraphBtn, &QPushButton::clicked, this, &MainWindow::openGraphWindow);
    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::handleStartStopButton);
    connect(saveDataBtn, &QPushButton::clicked, this, &MainWindow::saveDataToFile);

    // Rest of your existing controls setup...
    auto *controlsLayout = new QGridLayout();

    auto *distanceLabel = new QLabel("Distance:");
    distanceInput = new QLineEdit("00");
    distanceInput->setReadOnly(true); // Make the distance label non-editable
    auto *timeLabel = new QLabel("Time:");
    timeInput = new QTimeEdit();
    timeInput->setDisplayFormat("mm:ss.zzz"); // Set the display format to include milliseconds
    timeInput->setReadOnly(true); // Make the time label non-editable

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

void MainWindow::openPortSettings() const {
    portSettings->retranslateUi();
    portSettings->exec();
}

void MainWindow::openGraphWindow() {
    auto *graphWindow = new GraphWindow(this);
    graphWindow->show();
}

void MainWindow::saveDataToFile() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Data"), "", tr("CSV Files (*.csv)"));

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
                                                                  tr(
                                                                      "The file %1 already exists.\nDo you want to replace it?")
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
    out << "Distance;Time (mm:ss);Milliseconds;Raw Time (ms)\n";

    // Parse and write data from dataDisplay
    QString rawData = dataDisplay->toPlainText();
    QStringList lines = rawData.split('\n');
    QRegularExpression regex("YY(\\d+)T(\\d+)E");

    for (const QString &line: lines) {
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

            out << QString("%1;%2;%3;%4\n")
                    .arg(distance, timeFormatted)
                    .arg(milliseconds)
                    .arg(timeMs);
        }
    }

    file.close();

    QMessageBox::information(this, tr("Success"),
                             tr("Data has been saved to %1")
                             .arg(QDir::toNativeSeparators(fileName)));
}

void MainWindow::handleStartStopButton() {
    if (isReading) {
        stopReading();
        stopBtn->setText("Start");
        isReading = false;
    } else {
        // Try to start reading
        if (startReading()) {
            stopBtn->setText("Stop");
            isReading = true;
        }
    }
}


bool MainWindow::startReading() {
    QString portName = portSettings->getPortName();

    // Pre-validate the port name before attempting to open
    QList<QString> likelyInvalidPorts = {
        "wlan-debug", "Bluetooth", "iPhone", "iPad", "SOC", "debug"
    };

    bool isLikelyInvalid = false;
    for (const QString &pattern: likelyInvalidPorts) {
        if (portName.contains(pattern, Qt::CaseInsensitive)) {
            isLikelyInvalid = true;
            break;
        }
    }

    if (isLikelyInvalid) {
        QMessageBox::warning(this, "Inappropriate Device",
                             "The selected port \"" + portName + "\" does not appear to be a proper serial device.\n\n"
                             "This application requires a standard serial port, typically named:\n"
                             "• cu.usbserial-*\n"
                             "• tty.usbserial-*\n"
                             "• C0M*\n"
                             "• cu.usbmodem*\n"
                             "• cu.SLAB_*\n\n"
                             "Please select a different COM port in Port Settings.");
        return false;
    }

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

    // Create the serial port object
    serialPort = new QSerialPort(this);
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(static_cast<QSerialPort::DataBits>(dataBits));
    serialPort->setStopBits(static_cast<QSerialPort::StopBits>(stopBits));
    serialPort->setParity(static_cast<QSerialPort::Parity>(parity));
    serialPort->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl));

    // Try to open the port
    if (!serialPort->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Port Error",
                              "Could not open port " + portName + ".\n\n"
                              "This may be because:\n"
                              "• The port is already in use by another application\n"
                              "• You don't have sufficient permissions\n\n"
                              "Error: " + serialPort->errorString());
        delete serialPort;
        serialPort = nullptr;
        return false;
    }

    // Reset validation state
    deviceValidated = false;

    // Set up validation timer
    validationTimer = new QTimer(this);
    validationTimer->setSingleShot(true);
    validationTimer->start(VALIDATION_TIMEOUT);

    // Connect validation timeout handler
    connect(validationTimer, &QTimer::timeout, this, [this]() {
        QMessageBox::warning(this, "Incorrect Device",
                             "No valid data received from the device.\n\n"
                             "Expected data format: YY(distance)T(time)E\n\n"
                             "Please select a different COM port in Port Settings.");
        stopReading();
        isReading = false;
        stopBtn->setText("Start");
    });

    // Connect read signal
    connect(serialPort, &QSerialPort::readyRead, this, [this]() {
        QByteArray data = serialPort->readAll();
        dataDisplay->append(QString::fromUtf8(data));

        // Check if data matches our expected pattern
        const QRegularExpression regex("YY(\\d+)T(\\d+)E");
        QRegularExpressionMatch match = regex.match(QString::fromUtf8(data));

        if (match.hasMatch() && !deviceValidated) {
            // Valid data received - device is validated
            deviceValidated = true;
            if (validationTimer) {
                validationTimer->stop();
                delete validationTimer;
                validationTimer = nullptr;
            }
            Reading = true;
        }

        if (deviceValidated) {
            parseData(QString::fromUtf8(data));
        }
    });

    return true;
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
    if (validationTimer) {
        validationTimer->stop();
        delete validationTimer;
        validationTimer = nullptr;
    }

    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "Stopped reading from port";
        Reading = false;
        delete serialPort;
        serialPort = nullptr;
    }

    deviceValidated = false;
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F7) {
        dataDisplay->setVisible(!dataDisplay->isVisible());
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::loadLanguage(const QString &language) {
    // Remove previous translator if it exists
    if (translator) {
        QApplication::removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }

    translator = new QTranslator(this);
    QString translationFile = ":/translations/lidar_" + language + ".qm";
    bool loaded = translator->load(translationFile);

    if (!loaded) {
        qDebug() << "Failed to load translation file:" << translationFile;
    }

    if (loaded) {
        QApplication::installTranslator(translator);
    }

    // Update all UI elements in this window
    retranslateUi();

    // Update the main window's AppMenu
    appMenu->retranslateUi(); // Add this line
    appMenu->setLanguage(language);

    // Update all other windows
    for (QWidget *widget: QApplication::topLevelWidgets()) {
        if (auto *graphWindow = qobject_cast<GraphWindow *>(widget)) {
            graphWindow->retranslateUi();

            // Find and update the AppMenu in this GraphWindow
            for (QObject *child: graphWindow->children()) {
                if (auto *menu = qobject_cast<AppMenu *>(child)) {
                    menu->retranslateUi(); // Make sure this is called too
                    menu->setLanguage(language);
                }
            }
        }
    }

    // Update PortSettings if it exists
    if (portSettings) {
        portSettings->retranslateUi();
    }
}

// Add this new method to handle retranslation
void MainWindow::retranslateUi() {
    this->setWindowTitle(tr("EMPE"));
    appMenu->retranslateUi();

    // Buttons
    portSettingsBtn->setText(tr("Port settings"));
    showGraphBtn->setText(tr("Show Graph"));
    stopBtn->setText(isReading ? tr("Stop") : tr("Start"));
    saveDataBtn->setText(tr("Save data to file"));

    // Other UI elements
    alwaysOnTopCheckbox->setText(tr("Always on Top"));
    rawDataToggle->setText(tr("Get Raw Data"));
}
