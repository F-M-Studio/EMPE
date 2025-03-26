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

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, portSettings(new PortSettings(this))
,serialPort(nullptr)
,serialPort2(nullptr)
, isReading(false) {
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
    distanceInput->setReadOnly(true);
    auto *timeLabel = new QLabel("Time:");
    timeInput = new QTimeEdit();
    timeInput->setDisplayFormat("mm:ss.zzz");
    timeInput->setReadOnly(true);

    // Add second device controls
    auto *distance2Label = new QLabel("Distance 2:");
    distance2Input = new QLineEdit("00");
    distance2Input->setReadOnly(true);
    auto *time2Label = new QLabel("Time 2:");
    time2Input = new QTimeEdit();
    time2Input->setDisplayFormat("mm:ss.zzz");
    time2Input->setReadOnly(true);

    // First device controls (row 0 and 1)
    controlsLayout->addWidget(distanceLabel, 0, 0);
    controlsLayout->addWidget(distanceInput, 0, 1);
    controlsLayout->addWidget(timeLabel, 1, 0);
    controlsLayout->addWidget(timeInput, 1, 1);

    // Second device controls (row 0 and 1, column 2 and 3)
    controlsLayout->addWidget(distance2Label, 0, 2);
    controlsLayout->addWidget(distance2Input, 0, 3);
    controlsLayout->addWidget(time2Label, 1, 2);
    controlsLayout->addWidget(time2Input, 1, 3);

    mainLayout->addLayout(controlsLayout);

    // Hide second device controls by default
    distance2Label->setVisible(false);
    distance2Input->setVisible(false);
    time2Label->setVisible(false);
    time2Input->setVisible(false);

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

    // Check if the file name is empty
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, tr("No File Name"), tr("No file name was provided. Save operation was not performed."));
        return;
    }

    // Dodaj rozszerzenie .csv, jeśli nie jest obecne
    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    // Sprawdź, czy plik istnieje
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

    // Otwórz plik do zapisu
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Save Error"), tr("Cannot open the file for writing."));
        return;
    }

    // Zapisz dane do pliku (przykładowe dane)
    QTextStream out(&file);
    out << "Example data\n"; // Zastąp to swoimi danymi

    file.close();
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
    if (serialPort) {
        delete serialPort;
        serialPort = nullptr;
    }
    if (serialPort2) {
        delete serialPort2;
        serialPort2 = nullptr;
    }
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

    if (portSettings->isDualModeEnabled()) {
        QString port2Name = portSettings->getPort2Name();

        // Check if port2 is different from port1
        if (port2Name == portName) {
            QMessageBox::warning(this, "Port Error",
                                "Cannot use the same port for both devices in dual mode.");
            return false;
        }

        serialPort2 = new QSerialPort(this);
        serialPort2->setPortName(port2Name);
        // Use same configuration as first port
        serialPort2->setBaudRate(baudRate);
        serialPort2->setDataBits(static_cast<QSerialPort::DataBits>(dataBits));
        serialPort2->setStopBits(static_cast<QSerialPort::StopBits>(stopBits));
        serialPort2->setParity(static_cast<QSerialPort::Parity>(parity));
        serialPort2->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl));

        if (!serialPort2->open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Port Error",
                                "Failed to open second port " + port2Name + ".\n\n"
                                "Error: " + serialPort2->errorString());
            delete serialPort2;
            serialPort2 = nullptr;

            // Continue with just the first port
            isReading2 = false;
        } else {
            // Successfully opened second port
            isReading2 = true;

            // Connect read signal for second port
            connect(serialPort2, &QSerialPort::readyRead, this, [this]() {
                const QByteArray data = serialPort2->readAll();
                parseData2(QString::fromUtf8(data));
            });
        }
    }

    return true;
}

void MainWindow::parseData2(const QString &data) {
    const QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatch match = regex.match(data);

    if (match.hasMatch()) {
        bool ok1 = false, ok2 = false;
        distance2 = match.captured(1).toInt(&ok1);
        timeInMilliseconds2 = match.captured(2).toInt(&ok2);

        if (ok1 && ok2) {
            if (!useRawData) {
                static int lastValidDistance2 = 0;
                if (abs(distance2 - lastValidDistance2) > 8) {
                    distance2 = lastValidDistance2;
                }
                lastValidDistance2 = distance2;
            }

            // Only append valid data points
            dataPoints.append({distance2, timeInMilliseconds2});

            // Update UI
            distance2Input->setText(QString::number(distance2));
            int minutes2 = timeInMilliseconds2 / 60000;
            int seconds2 = timeInMilliseconds2 % 60000 / 1000;
            int milliseconds2 = timeInMilliseconds2 % 1000;
            time2Input->setTime(QTime(0, minutes2, seconds2, milliseconds2));

            // Make sure controls are visible
            distance2Input->parentWidget()->findChild<QLabel*>("Distance 2:")->setVisible(true);
            distance2Input->setVisible(true);
            time2Input->parentWidget()->findChild<QLabel*>("Time 2:")->setVisible(true);
            time2Input->setVisible(true);
        }
    }
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
            if (lastValidDistance == 0 || abs(newDistance - lastValidDistance) > 8) {
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

    // Only create and load new translator if not switching to English
    if (language != "en") {
        translator = new QTranslator(this);
        QString translationFile = ":/translations/lidar_" + language + ".qm";
        if (!translator->load(translationFile)) {
            qDebug() << "Failed to load translation file:" << translationFile;
            delete translator;
            translator = nullptr;
        } else {
            QApplication::installTranslator(translator);
        }
    }

    // Update all UI elements
    retranslateUi();
    appMenu->retranslateUi();
    appMenu->setLanguage(language);

    // Update all other windows
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (auto *graphWindow = qobject_cast<GraphWindow *>(widget)) {
            graphWindow->retranslateUi();
            // Update AppMenu in GraphWindow
            for (QObject *child : graphWindow->children()) {
                if (auto *menu = qobject_cast<AppMenu *>(child)) {
                    menu->retranslateUi();
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

void MainWindow::updateUIValues(int distance1, int time1, int distance2, int time2, bool showDevice2) {
    // Update first device values
    distanceInput->setText(QString::number(distance1));
    QTime time(0, time1 / 60000, (time1 % 60000) / 1000, time1 % 1000);
    timeInput->setTime(time);

    // Update second device values and visibility
    if (showDevice2) {
        distance2Input->setText(QString::number(distance2));
        QTime time2Obj(0, time2 / 60000, (time2 % 60000) / 1000, time2 % 1000);
        time2Input->setTime(time2Obj);

        // Make second device controls visible
        QLabel* distanceLabel = distance2Input->parentWidget()->findChild<QLabel*>("Distance 2:");
        if (distanceLabel) distanceLabel->setVisible(true);
        distance2Input->setVisible(true);

        QLabel* timeLabel = time2Input->parentWidget()->findChild<QLabel*>("Time 2:");
        if (timeLabel) timeLabel->setVisible(true);
        time2Input->setVisible(true);
    }
}