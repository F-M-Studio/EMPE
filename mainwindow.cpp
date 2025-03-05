#include "mainwindow.h"
#include <QDebug>
#include <QTimer>  // Include QTimer header

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isReading(false) {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    createMenu();    // Create hoverable menu
    createControls(); // Create bottom buttons & sliders
}

MainWindow::~MainWindow() {
    if (isReading) {
        stopReading();
    }
}

void MainWindow::createMenu() {
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    mainMenu = new QMenu("☰ Menu", this); // Compact menu
    menuBar->addMenu(mainMenu);

    // Create Actions
    portSettingsAction = new QAction("Port settings", this);
    graphAction = new QAction("Graph", this);
    startMeasurementAction = new QAction("Start measurement", this);
    saveDataAction = new QAction("Save data to file", this);

    // Add actions to menu
    mainMenu->addAction(portSettingsAction);
    mainMenu->addAction(graphAction);
    mainMenu->addAction(startMeasurementAction);
    mainMenu->addAction(saveDataAction);

    // Connect actions
    connect(portSettingsAction, &QAction::triggered, this, [this]() {
        PortSettings dialog(this);
        dialog.exec();
    });

    connect(graphAction, &QAction::triggered, this, []() {
        qDebug("Graph action triggered!");
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

    // Initialize portBox
    portBox = new QComboBox();
    portBox->addItem("COM1");
    portBox->addItem("COM2");
    portBox->addItem("COM3");
    // Add more COM ports as needed

    mainLayout->addWidget(portBox);  // Add portBox to the layout

    // Connect buttons to the same actions as the menu
    connect(portSettingsBtn, &QPushButton::clicked, this, [this]() {
        PortSettings dialog(this);
        dialog.exec();
    });

    connect(showGraphBtn, &QPushButton::clicked, this, []() {
        qDebug("Show Graph clicked!");
    });

    connect(stopBtn, &QPushButton::clicked, this, &MainWindow::handleStartStopButton);

    connect(saveDataBtn, &QPushButton::clicked, this, []() {
        qDebug("Save Data clicked!");
    });

    connect(clearGraphBtn, &QPushButton::clicked, this, []() {
        qDebug("Clear Graph clicked!");
    });

    // Controls layout
    QGridLayout *controlsLayout = new QGridLayout();

    QLabel *distanceLabel = new QLabel("Distance:");
    distanceInput = new QLineEdit("00");
    QLabel *timeLabel = new QLabel("Time:");
    timeInput = new QTimeEdit();
    timeInput->setDisabled(true);  // Make Time non-editable

    controlsLayout->addWidget(distanceLabel, 0, 0);
    controlsLayout->addWidget(distanceInput, 0, 1);
    controlsLayout->addWidget(timeLabel, 1, 0);
    controlsLayout->addWidget(timeInput, 1, 1);

    // Graph Sliders
    QLabel *yAxisLabel = new QLabel("Y axis scale:");
    yAxisSlider = new QSlider(Qt::Horizontal);
    yAxisSlider->setRange(0, 10000);  // Set range for Y axis slider

    controlsLayout->addWidget(yAxisLabel, 2, 0);
    controlsLayout->addWidget(yAxisSlider, 2, 1);

    QLabel *maxYLabel = new QLabel("max Y =");
    maxYInput = new QLineEdit("0");

    controlsLayout->addWidget(maxYLabel, 3, 0);
    controlsLayout->addWidget(maxYInput, 3, 1);

    // Recording period slider
    QLabel *recordingLabel = new QLabel("Recording period [ms]:");
    recordingSlider = new QSlider(Qt::Horizontal);
    recordingSlider->setRange(1, 1000);  // Set range for recording period slider
    recordingValueLabel = new QLabel("1");  // Label to display recording period value
    recordingValueLabel->setFixedWidth(50);  // Set fixed width for the label

    controlsLayout->addWidget(recordingLabel, 4, 0);
    controlsLayout->addWidget(recordingSlider, 4, 1);
    controlsLayout->addWidget(recordingValueLabel, 4, 2);  // Add label to layout

    mainLayout->addLayout(controlsLayout);

    // Connect sliders to update labels
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
    QString portName = portBox->currentText();
    if (serial.openDevice(portName.toStdString().c_str(), 115200) != 1) {
        qDebug() << "Failed to open port" << portName;
        return;
    }
    qDebug() << "Started reading from port" << portName;

    // Start a timer to read data periodically
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        char buffer[256];
        int bytesRead = serial.readBytes(buffer, sizeof(buffer) - 1, 100);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            qDebug() << "Data read:" << buffer;
        }
    });
    timer->start(100);  // Read data every 100 ms
}

void MainWindow::stopReading() {
    serial.closeDevice();
    qDebug() << "Stopped reading from port";
}