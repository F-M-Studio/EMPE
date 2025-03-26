#include "portsettings.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QVBoxLayout>

PortSettings::PortSettings(QWidget *parent)
    : QDialog(parent) {
    setupUI();
}

void PortSettings::setupUI() {
    setWindowTitle("Port Settings");
    setMinimumWidth(450);

    auto *mainLayout = new QVBoxLayout(this);

    // Create tab widget
    tabWidget = new QTabWidget(this);

    // Create port tabs
    QWidget *port1Tab = createPortTab("Port 1", portBox1, baudRateBox1, dataBitsBox1,
                                     stopBitsBox1, parityBox1, flowControlBox1);
    QWidget *port2Tab = createPortTab("Port 2", portBox2, baudRateBox2, dataBitsBox2,
                                     stopBitsBox2, parityBox2, flowControlBox2);

    tabWidget->addTab(port1Tab, "Port 1");
    tabWidget->addTab(port2Tab, "Port 2");

    mainLayout->addWidget(tabWidget);

    // Refresh Button
    refreshButton = new QPushButton("Refresh Port Lists");
    mainLayout->addWidget(refreshButton);
    connect(refreshButton, &QPushButton::clicked, this, &PortSettings::refreshPorts);

    // Buttons
    okButton = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect buttons
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setLayout(mainLayout);
    refreshPorts();
}

QWidget* PortSettings::createPortTab(const QString& tabName, QComboBox* &portBox,
                                    QComboBox* &baudRateBox, QComboBox* &dataBitsBox,
                                    QComboBox* &stopBitsBox, QComboBox* &parityBox,
                                    QComboBox* &flowControlBox) {
    QWidget *tab = new QWidget();
    auto *layout = new QGridLayout(tab);

    // Labels
    layout->addWidget(new QLabel("<b>" + tabName + " Settings</b>"), 0, 0, 1, 2);

    // Port Selection
    layout->addWidget(new QLabel("Port"), 1, 0);
    portBox = new QComboBox();
    layout->addWidget(portBox, 1, 1);

    // Baud Rate
    layout->addWidget(new QLabel("Baud rate"), 2, 0);
    baudRateBox = new QComboBox();
    baudRateBox->addItems({"9600", "19200", "38400", "57600", "115200"});
    baudRateBox->setCurrentText("115200");
    layout->addWidget(baudRateBox, 2, 1);

    // Data Bits
    layout->addWidget(new QLabel("Data bits"), 3, 0);
    dataBitsBox = new QComboBox();
    dataBitsBox->addItems({"6", "7", "8"});
    dataBitsBox->setCurrentText("8");
    layout->addWidget(dataBitsBox, 3, 1);

    // Stop Bits
    layout->addWidget(new QLabel("Stop bits"), 4, 0);
    stopBitsBox = new QComboBox();
    stopBitsBox->addItems({"1", "1.5", "2"});
    stopBitsBox->setCurrentText("1");
    layout->addWidget(stopBitsBox, 4, 1);

    // Parity
    layout->addWidget(new QLabel("Parity"), 5, 0);
    parityBox = new QComboBox();
    parityBox->addItems({"None", "Even", "Odd", "Mark", "Space"});
    parityBox->setCurrentText("None");
    layout->addWidget(parityBox, 5, 1);

    // Flow Control
    layout->addWidget(new QLabel("Flow control"), 6, 0);
    flowControlBox = new QComboBox();
    flowControlBox->addItems({"None", "Software", "Hardware"});
    flowControlBox->setCurrentText("Hardware");
    layout->addWidget(flowControlBox, 6, 1);

    return tab;
}

void PortSettings::refreshPorts() {
    QStringList portList;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portList << info.portName();
    }

    // Save current selections
    QString currentPort1 = portBox1->currentText();
    QString currentPort2 = portBox2->currentText();

    // Update port lists
    portBox1->clear();
    portBox2->clear();

    portBox1->addItems(portList);
    portBox2->addItems(portList);

    // Restore selections if possible
    int idx1 = portBox1->findText(currentPort1);
    int idx2 = portBox2->findText(currentPort2);

    if (idx1 >= 0) portBox1->setCurrentIndex(idx1);
    if (idx2 >= 0) portBox2->setCurrentIndex(idx2);

    // Select different ports if both are set to the same one
    if (portBox1->count() > 1 && portBox1->currentText() == portBox2->currentText()) {
        portBox2->setCurrentIndex((portBox1->currentIndex() + 1) % portBox2->count());
    }
}

QString PortSettings::getPortName1() const {
    return portBox1->currentText();
}

int PortSettings::getBaudRate1() const {
    return baudRateBox1->currentText().toInt();
}

int PortSettings::getDataBits1() const {
    return dataBitsBox1->currentText().toInt();
}

int PortSettings::getStopBits1() const {
    return stopBitsBox1->currentText().toInt();
}

int PortSettings::getParity1() const {
    return parityBox1->currentIndex();
}

int PortSettings::getFlowControl1() const {
    return flowControlBox1->currentIndex();
}

QString PortSettings::getPortName2() const {
    return portBox2->currentText();
}

int PortSettings::getBaudRate2() const {
    return baudRateBox2->currentText().toInt();
}

int PortSettings::getDataBits2() const {
    return dataBitsBox2->currentText().toInt();
}

int PortSettings::getStopBits2() const {
    return stopBitsBox2->currentText().toInt();
}

int PortSettings::getParity2() const {
    return parityBox2->currentIndex();
}

int PortSettings::getFlowControl2() const {
    return flowControlBox2->currentIndex();
}