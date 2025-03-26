#include "portsettings.h"
#include <QtSerialPort/QSerialPortInfo>
#include "mainwindow.h"


PortSettings::PortSettings(QWidget *parent) : QDialog(parent) {
    setupUI();
    refreshPorts();
    setWindowTitle("Port Settings");
    retranslateUi();

}

void PortSettings::retranslateUi() {
    setWindowTitle(tr("Setup"));

    refreshButton->setText(tr("Refresh"));
    okButton->setText(tr("OK"));
    cancelButton->setText(tr("Cancel"));

    const QMap<QString, QString> labelMap = {
        {"<b>Settings</b>", tr("<b>Settings</b>")},
        {"Port", tr("Port")},
        {"Baud", tr("Baud rate")},
        {"Data bits", tr("Data bits")},
        {"Stop bits", tr("Stop bits")},
        {"Parity", tr("Parity")},
        {"Flow", tr("Flow control")}
    };

    QList<QLabel *> labels = findChildren<QLabel *>();
    for (QLabel *label: labels) {
        const QString text = label->text();
        for (auto it = labelMap.begin(); it != labelMap.end(); ++it) {
            if (text.contains(it.key())) {
                label->setText(it.value());
                break;
            }
        }
    }

    // Update combo boxes that have translatable items
    const int parityIndex = parityBox->currentIndex();
    parityBox->clear();
    parityBox->addItems({tr("None"), tr("Even"), tr("Odd"), tr("Mark"), tr("Space")});
    parityBox->setCurrentIndex(parityIndex);

    const int flowIndex = flowControlBox->currentIndex();
    flowControlBox->clear();
    flowControlBox->addItems({tr("None"), tr("Software"), tr("Hardware")});
    flowControlBox->setCurrentIndex(flowIndex);
}
void PortSettings::setupUI() {
    auto *mainLayout = new QGridLayout(this);

    // Port selection
    auto *portLabel = new QLabel(tr("Select Port"), this);
    portBox = new QComboBox(this);

    // Dual mode elements
    dualModeCheckbox = new QCheckBox(tr("Enable Dual Mode"), this);
    auto *port2Label = new QLabel(tr("Select Port 2"), this);
    port2Box = new QComboBox(this);
    port2Box->setEnabled(false);

    // Serial port parameters
    auto *baudRateLabel = new QLabel(tr("Baud Rate"), this);
    baudRateBox = new QComboBox(this);
    auto *dataBitsLabel = new QLabel(tr("Data Bits"), this);
    dataBitsBox = new QComboBox(this);
    auto *stopBitsLabel = new QLabel(tr("Stop Bits"), this);
    stopBitsBox = new QComboBox(this);
    auto *parityLabel = new QLabel(tr("Parity"), this);
    parityBox = new QComboBox(this);
    auto *flowControlLabel = new QLabel(tr("Flow Control"), this);
    flowControlBox = new QComboBox(this);

    // Buttons
    auto *buttonsLayout = new QHBoxLayout;
    okButton = new QPushButton(tr("OK"), this);
    cancelButton = new QPushButton(tr("Cancel"), this);
    refreshButton = new QPushButton(tr("Refresh"), this);

    // Add widgets to layout
    mainLayout->addWidget(portLabel, 0, 0);
    mainLayout->addWidget(portBox, 0, 1);

    mainLayout->addWidget(dualModeCheckbox, 1, 0, 1, 2);
    mainLayout->addWidget(port2Label, 2, 0);
    mainLayout->addWidget(port2Box, 2, 1);

    mainLayout->addWidget(baudRateLabel, 3, 0);
    mainLayout->addWidget(baudRateBox, 3, 1);
    mainLayout->addWidget(dataBitsLabel, 4, 0);
    mainLayout->addWidget(dataBitsBox, 4, 1);
    mainLayout->addWidget(stopBitsLabel, 5, 0);
    mainLayout->addWidget(stopBitsBox, 5, 1);
    mainLayout->addWidget(parityLabel, 6, 0);
    mainLayout->addWidget(parityBox, 6, 1);
    mainLayout->addWidget(flowControlLabel, 7, 0);
    mainLayout->addWidget(flowControlBox, 7, 1);

    buttonsLayout->addWidget(refreshButton);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonsLayout, 8, 0, 1, 2);

    // Set up comboboxes with options
    const QList<qint32> baudRates = QSerialPortInfo::standardBaudRates();
    for (qint32 baudRate : baudRates) {
        baudRateBox->addItem(QString::number(baudRate), baudRate);
    }
    baudRateBox->setCurrentText("9600");

    QList<int> dataBits = {5, 6, 7, 8};
    for (int bits : dataBits) {
        dataBitsBox->addItem(QString::number(bits), bits);
    }
    dataBitsBox->setCurrentText("8");

    stopBitsBox->addItem("1", QSerialPort::OneStop);
    stopBitsBox->addItem("1.5", QSerialPort::OneAndHalfStop);
    stopBitsBox->addItem("2", QSerialPort::TwoStop);

    parityBox->addItem(tr("None"), QSerialPort::NoParity);
    parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

    // Connect signals and slots
    connect(refreshButton, &QPushButton::clicked, this, &PortSettings::refreshPorts);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(dualModeCheckbox, &QCheckBox::stateChanged, this, [this](int state) {
        port2Box->setEnabled(state == Qt::Checked);
    });

    // Initial port refresh
    refreshPorts();
}

void PortSettings::refreshPorts() const {
    portBox->clear();
    port2Box->clear();

    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port: ports) {
        portBox->addItem(port.portName());
        port2Box->addItem(port.portName());
    }
}

QString PortSettings::getPortName() const {
    return portBox->currentText();
}
QString PortSettings::getPort2Name() const {
    return port2Box->currentText();
}

bool PortSettings::isDualModeEnabled() const {
    return dualModeCheckbox->isChecked();
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
