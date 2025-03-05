#include "portsettings.h"
#include <QtSerialPort/QSerialPortInfo>

PortSettings::PortSettings(QWidget *parent)
    : QDialog(parent) {
    setupUI();
}

void PortSettings::setupUI() {
    setWindowTitle("Setup");

    QGridLayout *layout = new QGridLayout(this);

    // Labels
    layout->addWidget(new QLabel("<b>Settings</b>"), 0, 0, 1, 2);

    // Port Selection
    layout->addWidget(new QLabel("Port"), 1, 0);
    portBox = new QComboBox();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portBox->addItem(info.portName());
    }
    layout->addWidget(portBox, 1, 1);

    // Baud Rate
    layout->addWidget(new QLabel("Baud rate"), 2, 0);
    baudRateBox = new QComboBox();
    baudRateBox->addItems({"9600", "19200", "38400", "57600", "115200"});
    layout->addWidget(baudRateBox, 2, 1);

    // Data Bits
    layout->addWidget(new QLabel("Data bits"), 3, 0);
    dataBitsBox = new QComboBox();
    dataBitsBox->addItems({"6", "7", "8"});  // Remove "5"
    layout->addWidget(dataBitsBox, 3, 1);

    // Stop Bits
    layout->addWidget(new QLabel("Stop bits"), 4, 0);
    stopBitsBox = new QComboBox();
    stopBitsBox->addItems({"1", "1.5", "2"});
    layout->addWidget(stopBitsBox, 4, 1);

    // Parity
    layout->addWidget(new QLabel("Parity"), 5, 0);
    parityBox = new QComboBox();
    parityBox->addItems({"None", "Even", "Odd", "Mark", "Space"});
    layout->addWidget(parityBox, 5, 1);

    // Flow Control
    layout->addWidget(new QLabel("Flow control"), 6, 0);
    flowControlBox = new QComboBox();
    flowControlBox->addItems({"None", "Software", "Hardware"});
    layout->addWidget(flowControlBox, 6, 1);

    // Buttons
    okButton = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addLayout(buttonLayout, 7, 0, 1, 2);

    // Connect buttons
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setLayout(layout);
}

QString PortSettings::getPortName() const {
    return portBox->currentText();
}

int PortSettings::getBaudRate() const {
    return baudRateBox->currentText().toInt();
}

int PortSettings::getDataBits() const {
    return dataBitsBox->currentText().toInt();
}

int PortSettings::getStopBits() const {
    return stopBitsBox->currentText().toInt();
}

int PortSettings::getParity() const {
    return parityBox->currentIndex();
}

int PortSettings::getFlowControl() const {
    return flowControlBox->currentIndex();
}