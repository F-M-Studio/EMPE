/*
* Nazwa Projektu: EMPE
 * Plik: portsettings.cpp
 *
 * Krótki opis pliku: Implementacja klasy ustawień portów szeregowych umożliwiającej wykrywanie i konfigurację połączeń.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 4 Marca 2025
 * Ostatnia Modyfikacja: 18 Czerwaca 2025
 *
 * Ten program jest wolnym oprogramowaniem; możesz go rozprowadzać i/lub
 * modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
 * opublikowanej przez Free Software Foundation, w wersji 3 tej Licencji
 * lub (według twojego wyboru) dowolnej późniejszej wersji.
 *
 * Ten program jest rozpowszechniany w nadziei, że będzie użyteczny, ale
 * BEZ ŻADNEJ GWARANCJI; nawet bez domyślnej gwarancji PRZYDATNOŚCI
 * HANDLOWEJ lub PRZYDATNOŚCI DO OKREŚLONEGO CELU. Zobacz Powszechną
 * Licencję Publiczną GNU, aby uzyskać więcej szczegółów.
 *
 * Powinieneś otrzymać kopię Powszechnej Licencji Publicznej GNU wraz z
 * tym programem. Jeśli nie, zobacz <http://www.gnu.org/licenses/>.
*/

#include "portsettings.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QEvent>
#include <QDebug>

PortSettings::PortSettings(QWidget *parent) : QDialog(parent) {
    setupUI();
    retranslateUi();
}

void PortSettings::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void PortSettings::retranslateUi() {
    setWindowTitle(tr("Ustawienia"));
    if (refreshButton) refreshButton->setText(tr("Odśwież"));
    if (okButton) okButton->setText(tr("OK"));
    if (cancelButton) cancelButton->setText(tr("Anuluj"));
    if (tabWidget) {
        tabWidget->setTabText(0, tr("Port 1"));
        tabWidget->setTabText(1, tr("Port 2"));
    }
    // Etykiety i comboboxy w zakładkach
    if (portBox1) portBox1->setEditable(true); // by placeholder działał
    if (portBox2) portBox2->setEditable(true);
    if (portBox1) portBox1->setPlaceholderText(tr("Wybierz port"));
    if (portBox2) portBox2->setPlaceholderText(tr("Wybierz port"));
    if (baudRateBox1) baudRateBox1->setItemText(0, tr("9600"));
    if (baudRateBox1) baudRateBox1->setItemText(1, tr("19200"));
    if (baudRateBox1) baudRateBox1->setItemText(2, tr("38400"));
    if (baudRateBox1) baudRateBox1->setItemText(3, tr("57600"));
    if (baudRateBox1) baudRateBox1->setItemText(4, tr("115200"));
    if (baudRateBox2) baudRateBox2->setItemText(0, tr("9600"));
    if (baudRateBox2) baudRateBox2->setItemText(1, tr("19200"));
    if (baudRateBox2) baudRateBox2->setItemText(2, tr("38400"));
    if (baudRateBox2) baudRateBox2->setItemText(3, tr("57600"));
    if (baudRateBox2) baudRateBox2->setItemText(4, tr("115200"));
    if (dataBitsBox1) dataBitsBox1->setItemText(0, tr("6"));
    if (dataBitsBox1) dataBitsBox1->setItemText(1, tr("7"));
    if (dataBitsBox1) dataBitsBox1->setItemText(2, tr("8"));
    if (dataBitsBox2) dataBitsBox2->setItemText(0, tr("6"));
    if (dataBitsBox2) dataBitsBox2->setItemText(1, tr("7"));
    if (dataBitsBox2) dataBitsBox2->setItemText(2, tr("8"));
    if (stopBitsBox1) stopBitsBox1->setItemText(0, tr("1"));
    if (stopBitsBox1) stopBitsBox1->setItemText(1, tr("1.5"));
    if (stopBitsBox1) stopBitsBox1->setItemText(2, tr("2"));
    if (stopBitsBox2) stopBitsBox2->setItemText(0, tr("1"));
    if (stopBitsBox2) stopBitsBox2->setItemText(1, tr("1.5"));
    if (stopBitsBox2) stopBitsBox2->setItemText(2, tr("2"));
    // Parity i flowControl już obsłużone poniżej
    const int parityIndex1 = parityBox1 ? parityBox1->currentIndex() : 0;
    const int parityIndex2 = parityBox2 ? parityBox2->currentIndex() : 0;
    if (parityBox1) {
        parityBox1->clear();
        parityBox1->addItems({tr("None"), tr("Even"), tr("Odd"), tr("Mark"), tr("Space")});
        parityBox1->setCurrentIndex(parityIndex1);
    }
    if (parityBox2) {
        parityBox2->clear();
        parityBox2->addItems({tr("None"), tr("Even"), tr("Odd"), tr("Mark"), tr("Space")});
        parityBox2->setCurrentIndex(parityIndex2);
    }
    const int flowIndex1 = flowControlBox1 ? flowControlBox1->currentIndex() : 0;
    const int flowIndex2 = flowControlBox2 ? flowControlBox2->currentIndex() : 0;
    if (flowControlBox1) {
        flowControlBox1->clear();
        flowControlBox1->addItems({tr("None"), tr("Software"), tr("Hardware")});
        flowControlBox1->setCurrentIndex(flowIndex1);
    }
    if (flowControlBox2) {
        flowControlBox2->clear();
        flowControlBox2->addItems({tr("None"), tr("Software"), tr("Hardware")});
        flowControlBox2->setCurrentIndex(flowIndex2);
    }
    if (portInfoText) portInfoText->setPlaceholderText(tr("No serial ports detected"));
    // Info label
    // Jeśli infoLabel jest polem klasy, ustaw tekst:
    // if (infoLabel) infoLabel->setText(tr("<b>Connected Serial Ports Information:</b>"));
}

void PortSettings::setupUI() {
    setMinimumWidth(450);

    auto *mainLayout = new QVBoxLayout(this);


    tabWidget = new QTabWidget(this);


    QWidget *port1Tab = createPortTab(tr("Port 1"), portBox1, baudRateBox1, dataBitsBox1,
                                      stopBitsBox1, parityBox1, flowControlBox1);
    QWidget *port2Tab = createPortTab(tr("Port 2"), portBox2, baudRateBox2, dataBitsBox2,
                                      stopBitsBox2, parityBox2, flowControlBox2);

    tabWidget->addTab(port1Tab, tr("Port 1"));
    tabWidget->addTab(port2Tab, tr("Port 2"));

    mainLayout->addWidget(tabWidget);

    refreshButton = new QPushButton(this);
    mainLayout->addWidget(refreshButton);
    connect(refreshButton, &QPushButton::clicked, this, &PortSettings::refreshPorts);


    QLabel *infoLabel = new QLabel(tr("<b>Connected Serial Ports Information:</b>"));
    mainLayout->addWidget(infoLabel);

    portInfoText = new QTextEdit(this);
    portInfoText->setReadOnly(true);
    portInfoText->setMinimumHeight(150);
    mainLayout->addWidget(portInfoText);


    okButton = new QPushButton(this);
    cancelButton = new QPushButton(this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);


    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setLayout(mainLayout);
    refreshPorts();
}

QWidget *PortSettings::createPortTab(const QString &tabName, QComboBox * &portBox,
                                     QComboBox * &baudRateBox, QComboBox * &dataBitsBox,
                                     QComboBox * &stopBitsBox, QComboBox * &parityBox,
                                     QComboBox * &flowControlBox) {
    QWidget *tab = new QWidget();
    auto *layout = new QGridLayout(tab);


    layout->addWidget(new QLabel(QString("<b>%1 %2</b>").arg(tabName, tr("Settings"))), 0, 0, 1, 2);


    layout->addWidget(new QLabel(tr("Port")), 1, 0);
    portBox = new QComboBox();
    layout->addWidget(portBox, 1, 1);


    layout->addWidget(new QLabel(tr("Baud rate")), 2, 0);
    baudRateBox = new QComboBox();
    baudRateBox->addItems({tr("9600"), tr("19200"), tr("38400"), tr("57600"), tr("115200")});
    baudRateBox->setCurrentText(tr("115200"));
    layout->addWidget(baudRateBox, 2, 1);


    layout->addWidget(new QLabel(tr("Data bits")), 3, 0);
    dataBitsBox = new QComboBox();
    dataBitsBox->addItems({tr("6"), tr("7"), tr("8")});
    dataBitsBox->setCurrentText(tr("8"));
    layout->addWidget(dataBitsBox, 3, 1);


    layout->addWidget(new QLabel(tr("Stop bits")), 4, 0);
    stopBitsBox = new QComboBox();
    stopBitsBox->addItems({tr("1"), tr("1.5"), tr("2")});
    stopBitsBox->setCurrentText(tr("1"));
    layout->addWidget(stopBitsBox, 4, 1);


    layout->addWidget(new QLabel(tr("Parity")), 5, 0);
    parityBox = new QComboBox();
    parityBox->addItems({tr("None"), tr("Even"), tr("Odd"), tr("Mark"), tr("Space")});
    parityBox->setCurrentText(tr("None"));
    layout->addWidget(parityBox, 5, 1);

    layout->addWidget(new QLabel(tr("Flow control")), 6, 0);
    flowControlBox = new QComboBox();
    flowControlBox->addItems({tr("None"), tr("Software"), tr("Hardware")});
    flowControlBox->setCurrentText(tr("Hardware"));
    layout->addWidget(flowControlBox, 6, 1);

    return tab;
}

void PortSettings::refreshPorts() {
    QStringList portList;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portList << info.portName();
    }


    QString currentPort1 = portBox1->currentText();
    QString currentPort2 = portBox2->currentText();


    portBox1->clear();
    portBox2->clear();

    portBox1->addItems(portList);
    portBox2->addItems(portList);


    int idx1 = portBox1->findText(currentPort1);
    int idx2 = portBox2->findText(currentPort2);

    if (idx1 >= 0) portBox1->setCurrentIndex(idx1);
    if (idx2 >= 0) portBox2->setCurrentIndex(idx2);

    if (portBox1->count() > 1 && portBox1->currentText() == portBox2->currentText()) {
        portBox2->setCurrentIndex((portBox1->currentIndex() + 1) % portBox2->count());
    }

    updatePortInfo();
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

void PortSettings::updatePortInfo() {
    portInfoText->clear();

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty()) {
        portInfoText->setText(tr("No serial ports detected"));
        return;
    }

    QString info;
    for (const QSerialPortInfo &port: ports) {

        bool isBluetooth = port.description().contains("Bluetooth", Qt::CaseInsensitive) ||
                           port.manufacturer().contains("Bluetooth", Qt::CaseInsensitive) ||
                           port.portName().contains("BT", Qt::CaseInsensitive) ||
                           port.description().contains("empesensor", Qt::CaseInsensitive) ||
                           port.manufacturer().contains("empesensor", Qt::CaseInsensitive) ||
                           port.portName().contains("empesensor", Qt::CaseInsensitive);

        if (isBluetooth) {
            info += tr("<b style='color:blue;'>Bluetooth Device</b><br>");
        }

        info += tr("<b>Port:</b> %1<br>").arg(port.portName());
        info += tr("<b>Description:</b> %1<br>").arg(port.description().isEmpty() ? tr("N/A") : port.description());
        info += tr("<b>Manufacturer:</b> %1<br>").arg(port.manufacturer().isEmpty() ? tr("N/A") : port.manufacturer());
        info += tr("<b>Serial Number:</b> %1<br>").arg(port.serialNumber().isEmpty() ? tr("N/A") : port.serialNumber());
        info += tr("<b>Location:</b> %1<br>").arg(port.systemLocation());
        //jakaś stara implementacja
        /*
        info += tr("<b>Vendor ID:</b> %1<br>").arg(port.hasVendorIdentifier()
                                                       ? QString("0x%1").arg(port.vendorIdentifier(), 4, 16, QChar('0'))
                                                       : tr("N/A"));
        info += tr("<b>Product ID:</b> %1<br>").arg(port.hasProductIdentifier()
                                                        ? QString("0x%1").arg(
                                                            port.productIdentifier(), 4, 16, QChar('0'))
                                                        : tr("N/A"));
                                                        */
        info += "<br>";
    }

    portInfoText->setHtml(info);
}