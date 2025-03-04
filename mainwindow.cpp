#include "mainwindow.h"
#include <QVBoxLayout>
#include "portsettings.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    setupUI();
    applyStyles();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Top menu buttons (removed duplicates)
    QHBoxLayout *menuLayout = new QHBoxLayout();
    menuLayout->addWidget(new QPushButton("Port settings"));
    menuLayout->addWidget(new QPushButton("Graph"));
    menuLayout->addWidget(new QPushButton("Start measurement"));
    menuLayout->addWidget(new QPushButton("Save data to file"));
    mainLayout->addLayout(menuLayout);

    // Control Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    portSettingsBtn = new QPushButton("PORT settings");
    showGraphBtn = new QPushButton("Show GRAPH");
    stopBtn = new QPushButton("STOP");
    saveDataBtn = new QPushButton("SAVE data to file");
    clearGraphBtn = new QPushButton("Clear GRAPH");

    buttonLayout->addWidget(portSettingsBtn);
    buttonLayout->addWidget(showGraphBtn);
    buttonLayout->addWidget(stopBtn);
    buttonLayout->addWidget(saveDataBtn);
    buttonLayout->addWidget(clearGraphBtn);
    mainLayout->addLayout(buttonLayout);

    // Distance & Time Section
    QGridLayout *formLayout = new QGridLayout();
    distanceLabel = new QLabel("Distance");
    distanceInput = new QLineEdit("00");
    timeLabel = new QLabel("Time");
    timeInput = new QTimeEdit();
    timeInput->setDisabled(true);  // Make Time non-editable

    formLayout->addWidget(distanceLabel, 0, 0);
    formLayout->addWidget(distanceInput, 0, 1);
    formLayout->addWidget(timeLabel, 1, 0);
    formLayout->addWidget(timeInput, 1, 1);
    mainLayout->addLayout(formLayout);

    // Sliders Section
    QGroupBox *sliderGroup = new QGroupBox("Graph Controls");
    QGridLayout *sliderLayout = new QGridLayout();

    yAxisLabel = new QLabel("Y axis scale");
    yAxisSlider = new QSlider(Qt::Horizontal);
    maxYLabel = new QLabel("max Y =");
    maxYInput = new QLineEdit("0");

    recordingLabel = new QLabel("Recording period [ms]");
    recordingSlider = new QSlider(Qt::Horizontal);

    sliderLayout->addWidget(yAxisLabel, 0, 0);
    sliderLayout->addWidget(yAxisSlider, 0, 1);
    sliderLayout->addWidget(maxYLabel, 1, 0);
    sliderLayout->addWidget(maxYInput, 1, 1);
    sliderLayout->addWidget(recordingLabel, 2, 0);
    sliderLayout->addWidget(recordingSlider, 2, 1);

    sliderGroup->setLayout(sliderLayout);
    mainLayout->addWidget(sliderGroup);
    connect(portSettingsBtn, &QPushButton::clicked, this, [this]() {
    PortSettings dialog(this);
    dialog.exec();  // Show as modal dialog
});

}


void MainWindow::applyStyles() {
    setStyleSheet(R"(
        QWidget {
            background-color: #2E2E2E;
            color: white;
            font-size: 14px;
        }
        QPushButton {
            background-color: #555;
            border: 1px solid #777;
            padding: 8px;
            border-radius: 5px;
        }
        QPushButton:hover {
            background-color: #777;
        }
        QLineEdit, QTimeEdit {
            background-color: #444;
            border: 1px solid #666;
            padding: 5px;
            border-radius: 3px;
        }
        QSlider::groove:horizontal {
            height: 8px;
            background: #666;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #888;
            width: 18px;
            margin: -5px 0;
            border-radius: 9px;
        }
    )");
}
//
// Created by macho on 4.03.2025.
//
