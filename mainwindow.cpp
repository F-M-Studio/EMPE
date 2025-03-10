// mainwindow.cpp

#include "mainwindow.h"
#include "graphwindow.h"
#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isReading(false), portSettings(new PortSettings(this)) {
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
    stopBtn = new QPushButton("START");
    saveDataBtn = new QPushButton("SAVE data to file");
    clearGraphBtn = new QPushButton("Clear GRAPH");

    buttonLayout->addWidget(portSettingsBtn);
    buttonLayout->addWidget(showGraphBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(saveDataBtn);
    buttonLayout->addWidget(clearGraphBtn);

    mainLayout->addLayout(buttonLayout);

    connect(portSettingsBtn, &QPushButton::clicked, this, [this]() {
        portSettings->exec();
    });

    connect(showGraphBtn, &QPushButton::clicked, this, [this]() {
        GraphWindow *graphWindow = new GraphWindow(this);
        graphWindow->show();
    });

    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::handleStartStopButton);

    connect(saveDataBtn, &QPushButton::clicked, this, []() {
        qDebug("Save Data clicked!");
    });

    connect(clearGraphBtn, &QPushButton::clicked, this, []() {
        qDebug("Clear Graph clicked!");
    });

    QGridLayout *controlsLayout = new QGridLayout();

    QLabel *distanceLabel = new QLabel("Distance:");
    distanceInput = new QLineEdit("00");
    QLabel *timeLabel = new QLabel("Time:");
    timeInput = new QTimeEdit();
    timeInput->setDisabled(true);

    controlsLayout->addWidget(distanceLabel, 0, 0);
    controlsLayout->addWidget(distanceInput, 0, 1);
    controlsLayout->addWidget(timeLabel, 1, 0);
    controlsLayout->addWidget(timeInput, 1, 1);

    QLabel *yAxisLabel = new QLabel("Y axis scale:");
    yAxisSlider = new QSlider(Qt::Horizontal);
    yAxisSlider->setRange(0, 10000);

    controlsLayout->addWidget(yAxisLabel, 2, 0);
    controlsLayout->addWidget(yAxisSlider, 2, 1);

    QLabel *maxYLabel = new QLabel("max Y =");
    maxYInput = new QLineEdit("0");

    controlsLayout->addWidget(maxYLabel, 3, 0);
    controlsLayout->addWidget(maxYInput, 3, 1);

    QLabel *recordingLabel = new QLabel("Recording period [ms]:");
    recordingSlider = new QSlider(Qt::Horizontal);
    recordingSlider->setRange(1, 1000);
    recordingValueLabel = new QLabel("1");
    recordingValueLabel->setFixedWidth(50);

    controlsLayout->addWidget(recordingLabel, 4, 0);
    controlsLayout->addWidget(recordingSlider, 4, 1);
    controlsLayout->addWidget(recordingValueLabel, 4, 2);

    mainLayout->addLayout(controlsLayout);

    connect(yAxisSlider, &QSlider::valueChanged, this, [this](int value) {
        maxYInput->setText(QString::number(value));
    });

    connect(recordingSlider, &QSlider::valueChanged, this, [this](int value) {
        recordingValueLabel->setText(QString::number(value));
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

    QSerialPort *serialPort = new QSerialPort(this);
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(static_cast<QSerialPort::DataBits>(dataBits));
    serialPort->setStopBits(static_cast<QSerialPort::StopBits>(stopBits));
    serialPort->setParity(static_cast<QSerialPort::Parity>(parity));
    serialPort->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl));

    if (!serialPort->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open port" << portName << "Error:" << serialPort->errorString();
        delete serialPort;
        return;
    }

    qDebug() << "Started reading from port" << portName;

    connect(serialPort, &QSerialPort::readyRead, this, [this, serialPort]() {
        QByteArray data = serialPort->readAll();
        qDebug() << "Data read:" << data;
        dataDisplay->append(data);
    });
}

void MainWindow::stopReading() {
    int i = 1;

    // if (serialPort && serialPort->isOpen()) {
    //     serialPort->close();
    //     qDebug() << "Stopped reading from port";
    //     delete serialPort;
    //     serialPort = nullptr;
    // }
}
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F7) {
        dataDisplay->setVisible(!dataDisplay->isVisible());
    } else {
        QMainWindow::keyPressEvent(event);
    }
}