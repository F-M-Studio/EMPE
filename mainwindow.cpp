/*
* Nazwa Projektu: EMPE
 * Plik: mainwindow.cpp
 *
 * Krótki opis pliku: inicjalizacja i elementy logiki głównego okna aplikacji
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 4 Marca 2025
 * Ostatnia Modyfikacja: 18 Czerwca 2025
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

#include "mainwindow.h"
#include "portsettings.h"
#include "appmenu.h"
#include "graphwindow.h"
#include "aboutusdialog.h"

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
#include <QApplication>
#include <QPixmap>
#include <QGroupBox>
#include <QGridLayout>
#include <QDateTime>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), isReading(false),
      portSettings(new PortSettings(this)) {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setWindowTitle(tr("EMPE - System Pomiarowy"));


    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);


    appMenu = new AppMenu(this, this);
    createControls();
    createStoperControls();

    QWidget* dataContainer = new QWidget(this);
    QVBoxLayout* dataLayout = new QVBoxLayout(dataContainer);
    dataLayout->setContentsMargins(0, 0, 0, 0);
    dataLayout->setSpacing(0);

    dataDisplay = new QTextEdit(this);
    dataDisplay->setReadOnly(true);
    dataDisplay->hide();
    dataDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataLayout->addWidget(dataDisplay);

    dataDisplay2 = new QTextEdit(this);
    dataDisplay2->setReadOnly(true);
    dataDisplay2->hide();
    dataDisplay2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataLayout->addWidget(dataDisplay2);


    dataContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(dataContainer, 1);

    connect(appMenu, &AppMenu::graphWindowRequested, this, [this]() {
        GraphWindow *graphWindow = new GraphWindow(this);
        graphWindow->show();
    });
    connect(appMenu, &AppMenu::portSettingsRequested, this, [this]() {
        PortSettings *portSettings = new PortSettings(this);
        portSettings->show();
    });
    connect(appMenu, &AppMenu::aboutUsRequested, this, &MainWindow::showAboutUsDialog);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();

    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap(":/images/FundedByEU.png");
    imageLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    bottomLayout->addWidget(imageLabel);
    mainLayout->addLayout(bottomLayout);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);

    creatorsNoteLabel = new QLabel(tr("Program powstał w ramach projektu Embodying Math&Physics Education 2023-1-PL01-KA210-SCH-000165829"),
                                  this);
    creatorsNoteLabel->setAlignment(Qt::AlignCenter);
    QFont noteFont = creatorsNoteLabel->font();
    noteFont.setPointSize(noteFont.pointSize() - 1);
    noteFont.setItalic(true);
    creatorsNoteLabel->setFont(noteFont);
    creatorsNoteLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    creatorsNoteLabel->setFixedHeight(creatorsNoteLabel->sizeHint().height());
    mainLayout->addWidget(creatorsNoteLabel);
}

MainWindow::~MainWindow() {
    if (isReading) {
        stopReading();
    }
}

void MainWindow::createStoperControls() {

    stoperGroupBox = new QGroupBox(this);
    QGridLayout *stoperLayout = new QGridLayout(stoperGroupBox);

    QLabel *sensLabel = new QLabel(tr("Czułość spadku (mm):"), this);
    sensitivitySlider = new QSlider(Qt::Horizontal, this);
    sensitivitySlider->setRange(5, 100);
    sensitivitySlider->setValue(dropSensitivity);
    sensitivitySlider->setTickPosition(QSlider::TicksBelow);
    sensitivitySlider->setTickInterval(5);

    sensitivityLabel = new QLabel(QString::number(dropSensitivity), this);
    sensitivityLabel->setMinimumWidth(5);
    resetStoperBtn = new QPushButton(tr("Resetuj"), this);


    stoperTimer = new QTimer(this);
    stoperTimer->setInterval(10);
    stoperTime = 60000;
    stoperRunning = false;


    stoperLayout->addWidget(sensLabel, 0, 0);
    stoperLayout->addWidget(sensitivitySlider, 0, 1, 1, 2);
    stoperLayout->addWidget(sensitivityLabel, 0, 3);


    QGroupBox *sensor1GroupBox = new QGroupBox(tr("Czujnik 1"), this);
    QVBoxLayout *sensor1Layout = new QVBoxLayout(sensor1GroupBox);


    enableStoper1CheckBox = new QCheckBox(tr("Włącz czujnik 1"), this);
    enableStoper1CheckBox->setChecked(stoper1Enabled);
    sensor1Layout->addWidget(enableStoper1CheckBox);


    dropCounter1Label = new QLabel(tr("Spadki: 0"), this);
    dropCounter1Label->setAlignment(Qt::AlignCenter);
    sensor1Layout->addWidget(dropCounter1Label);


    stopwatchLabel1 = new QLabel(tr("Stoper: 00:00.000"));
    stopwatchLabel1->setAlignment(Qt::AlignCenter);
    stopwatchTimer1 = new QTimer(this);
    stopwatchTimer1->setInterval(10);
    connect(stopwatchTimer1, &QTimer::timeout, this, &MainWindow::updateStopwatch1);
    sensor1Layout->addWidget(stopwatchLabel1);


    QGroupBox *sensor2GroupBox = new QGroupBox(tr("Czujnik 2"), this);
    QVBoxLayout *sensor2Layout = new QVBoxLayout(sensor2GroupBox);


    enableStoper2CheckBox = new QCheckBox(tr("Włącz czujnik 2"), this);
    enableStoper2CheckBox->setChecked(stoper2Enabled);
    sensor2Layout->addWidget(enableStoper2CheckBox);


    dropCounter2Label = new QLabel(tr("Spadki: 0"), this);
    dropCounter2Label->setAlignment(Qt::AlignCenter);
    sensor2Layout->addWidget(dropCounter2Label);


    stopwatchLabel2 = new QLabel(tr("Stoper: 00:00.000"));
    stopwatchLabel2->setAlignment(Qt::AlignCenter);
    stopwatchTimer2 = new QTimer(this);
    stopwatchTimer2->setInterval(10);
    connect(stopwatchTimer2, &QTimer::timeout, this, &MainWindow::updateStopwatch2);
    sensor2Layout->addWidget(stopwatchLabel2);


    stoperLayout->addWidget(sensor1GroupBox, 1, 0, 1, 2);
    stoperLayout->addWidget(sensor2GroupBox, 1, 2, 1, 2);


    stoperLayout->addWidget(resetStoperBtn, 2, 0, 1, 4);


    mainLayout->addWidget(stoperGroupBox);


    connect(stoperTimer, &QTimer::timeout, this, &MainWindow::updateStoperTime);
    connect(sensitivitySlider, &QSlider::valueChanged, this, &MainWindow::onSensitivityChanged);
    connect(resetStoperBtn, &QPushButton::clicked, this, &MainWindow::resetStoperCounters);
    connect(enableStoper1CheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        stoper1Enabled = checked;
    });
    connect(enableStoper2CheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        stoper2Enabled = checked;
    });
}

void MainWindow::handleStoperStartStop() {
    if (!stoperRunning) {
        stoperRunning = true;
        startStopStoperBtn->setText(tr("Zatrzymaj stoper"));
        stoperTimer->start();
    } else {
        stoperRunning = false;
        startStopStoperBtn->setText(tr("Uruchom stoper"));
        stoperTimer->stop();
    }
}
void MainWindow::updateStoperTime() {
    if (stoperTime > 0) {
        stoperTime -= 10;
        int mins = stoperTime / 60000;
        int secs = (stoperTime % 60000) / 1000;
        int ms = (stoperTime % 1000) / 10;

        QString timeStr = QString("%1:%2.%3")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'))
            .arg(ms, 2, 10, QChar('0'));

        startStopStoperBtn->setText(tr("Zatrzymaj stoper (%1)").arg(timeStr));
    } else {
        stoperTimer->stop();
        stoperRunning = false;
        startStopStoperBtn->setText(tr("Uruchom stoper"));
        QMessageBox::information(this, tr("Stoper"), tr("Koniec czasu!"));
    }
}
void MainWindow::resetStoperCounters() {
    dropCount1 = 0;
    dropCount2 = 0;
    dropEvents.clear();
    previousDistance1 = 0;
    previousDistance2 = 0;
    lastDropTime1 = QDateTime();
    lastDropTime2 = QDateTime();
    stoperTime = 60000;

    if (stoperRunning) {
        stoperTimer->stop();
        stoperRunning = false;
    }


    resetStopwatch1();
    resetStopwatch2();

    updateStoperDisplay();

    QMessageBox::information(this, tr("Reset zakończony"),
                           tr("Liczniki spadków, logi i stoper zostały zresetowane."));
}

void MainWindow::saveStoperLogs() {
    if (dropEvents.isEmpty()) {
        QMessageBox::information(this, tr("Brak danych"),
                               tr("Brak zdarzeń spadku do zapisania."));
        return;
    }

    QString defaultFileName = QString("EMPE_DropLogs_%1.csv")
                             .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Zapisz logi spadków"),
                                                  defaultFileName,
                                                  tr("Pliki CSV (*.csv)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Błąd"),
                           tr("Nie można zapisać pliku %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName),
                           file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << "Timestamp,Sensor,Previous Value (mm),Current Value (mm),Drop Amount (mm),Sensitivity (mm)\n";


    for (const auto &event : dropEvents) {
        out << QString("%1,%2,%3,%4,%5,%6\n")
               .arg(event.timestamp.toString("yyyy-MM-dd HH:mm:ss.zzz"))
               .arg(event.sensorNumber)
               .arg(event.previousValue)
               .arg(event.currentValue)
               .arg(event.dropAmount)
               .arg(dropSensitivity);
    }

    file.close();

    QMessageBox::information(this, tr("Sukces"),
                           tr("Logi spadków zapisane w %1\nLiczba zdarzeń: %2")
                           .arg(QDir::toNativeSeparators(fileName))
                           .arg(dropEvents.size()));
}

void MainWindow::checkForDrop1(int currentDistance) {
    if (!stoper1Enabled) return;


    int difference = previousDistance1 - currentDistance;


    if (difference >= dropSensitivity) {
        dropCount1++;


        logDropEvent(1, previousDistance1, currentDistance, difference);


        if (!stopwatchRunning1) {

            stopwatchRunning1 = true;
            stopwatchTimer1->start(10);

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay->append(tr("[%1] Automatyczne uruchomienie stopera 1 po wykryciu spadku")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie uruchomiono stoper 1 po wykryciu spadku";
        } else {

            stopwatchRunning1 = false;
            stopwatchTimer1->stop();

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay->append(tr("[%1] Automatyczne zatrzymanie stopera 1 po wykryciu spadku")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie zatrzymano stoper 1 po wykryciu spadku";
        }


        updateStoperDisplay();


        lastDropTime1 = QDateTime::currentDateTime();
    }


    previousDistance1 = currentDistance;
}

void MainWindow::checkForDrop2(int currentDistance) {
    if (!stoper2Enabled) return;


    int difference = previousDistance2 - currentDistance;


    if (difference >= dropSensitivity) {

        dropCount2++;


        logDropEvent(2, previousDistance2, currentDistance, difference);


        if (!stopwatchRunning2) {

            stopwatchRunning2 = true;
            stopwatchTimer2->start(10);

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay2->append(tr("[%1] Automatyczne uruchomienie stopera 2 po wykryciu spadku")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie uruchomiono stoper 2 po wykryciu spadku";
        } else {

            stopwatchRunning2 = false;
            stopwatchTimer2->stop();

            QDateTime currentTime = QDateTime::currentDateTime();
            dataDisplay2->append(tr("[%1] Automatyczne zatrzymanie stopera 2 po wykryciu spadku")
                .arg(currentTime.toString("hh:mm:ss.zzz")));
            qDebug() << "Automatycznie zatrzymano stoper 2 po wykryciu spadku";
        }


        updateStoperDisplay();


        lastDropTime2 = QDateTime::currentDateTime();
    }


    previousDistance2 = currentDistance;
}

void MainWindow::updateStoperDisplay() {
    dropCounter1Label->setText(tr("Spadki: %1").arg(dropCount1));
    dropCounter2Label->setText(tr("Spadki: %1").arg(dropCount2));
}

void MainWindow::handleStartStopButton() {
    if (isReading) {
        stopReading();
        startStopBtn->setText(tr("START"));
    } else {
        startReading();
        startStopBtn->setText(tr("STOP"));
    }
    isReading = !isReading;
}

void MainWindow::createControls() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    portSettingsBtn = new QPushButton(tr("Ustawienia portu"));
    showGraphBtn = new QPushButton(tr("Pokaż wykres"));
    startStopBtn = new QPushButton(tr("START"));
    saveDataBtn = new QPushButton(tr("Zapisz dane 1"));
    saveData2Btn = new QPushButton(tr("Zapisz dane 2"));
    clearGraphBtn = new QPushButton(tr("Wyczyść wykres"));
    showRawDataBtn = new QPushButton(tr("Pokaż surowe dane"));

    buttonLayout->addWidget(portSettingsBtn);
    buttonLayout->addWidget(showGraphBtn);
    buttonLayout->addWidget(startStopBtn);
    buttonLayout->addWidget(saveDataBtn);
    buttonLayout->addWidget(saveData2Btn);
    buttonLayout->addWidget(clearGraphBtn);
    buttonLayout->addWidget(showRawDataBtn);

    mainLayout->addLayout(buttonLayout);

    enableSecondLidarCheckBox = new QCheckBox(tr("Włącz drugi lidar"), this);
    enableSecondLidarCheckBox->setChecked(false);
    mainLayout->addWidget(enableSecondLidarCheckBox);
    connect(enableSecondLidarCheckBox, &QCheckBox::toggled, this, &MainWindow::onEnableSecondLidarToggled);

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

    connect(showRawDataBtn, &QPushButton::clicked, this, [this]() {
        bool visible = !dataDisplay->isVisible();
        dataDisplay->setVisible(visible);
        dataDisplay2->setVisible(visible);
        showRawDataBtn->setText(visible ? tr("Ukryj surowe dane") : tr("Pokaż surowe dane"));

        if (!visible) {
            setMinimumSize(0, 0);
            resize(600, 200);
            setMinimumSize(minimumSizeHint());
        }else {
            adjustSize();
        }
    });

    QGridLayout *controlsLayout = new QGridLayout();


    QGroupBox *sensor1Box = new QGroupBox(tr("Czujnik 1"));
    QVBoxLayout *sensor1Layout = new QVBoxLayout(sensor1Box);

    QLabel *distanceLabel = new QLabel(tr("Odległość:"));
    distanceInput = new QLineEdit("00");
    distanceInput->setReadOnly(true);

    sensor1Layout->addWidget(distanceLabel);
    sensor1Layout->addWidget(distanceInput);


    QGroupBox *sensor2Box = new QGroupBox(tr("Czujnik 2"));
    QVBoxLayout *sensor2Layout = new QVBoxLayout(sensor2Box);

    QLabel *distanceLabel2 = new QLabel(tr("Odległość:"));
    distanceInput2 = new QLineEdit("00");
    distanceInput2->setReadOnly(true);

    sensor2Layout->addWidget(distanceLabel2);
    sensor2Layout->addWidget(distanceInput2);

    QGroupBox *timeBox = new QGroupBox(tr("Czas globalny"));
    QVBoxLayout *timeLayout = new QVBoxLayout(timeBox);

    globalTimeLabel = new QLabel("00:00.000");
    globalTimeLabel->setAlignment(Qt::AlignCenter);
    QFont timeFont = globalTimeLabel->font();
    timeFont.setPointSize(timeFont.pointSize() + 2);
    globalTimeLabel->setFont(timeFont);

    timeLayout->addWidget(globalTimeLabel);

    controlsLayout->addWidget(sensor1Box, 0, 0);
    controlsLayout->addWidget(sensor2Box, 0, 1);
    controlsLayout->addWidget(timeBox, 1, 0, 1, 2);

    controlsLayout->addWidget(sensor1Box, 0, 0);
    controlsLayout->addWidget(sensor2Box, 0, 1);

    mainLayout->addLayout(controlsLayout);

    alwaysOnTopCheckbox = new QCheckBox(tr("Zawsze na wierzchu"), this);
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

void MainWindow::saveDataToFile(const QTextEdit *display, const QString &regexPattern) {
    QString defaultFileName = QString("EMPE_%1.csv").arg(QDate::currentDate().toString("ddMMyyyy"));
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Zapisz dane"), defaultFileName,
                                                    tr("Pliki CSV (*.csv)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

    QFile file(fileName);
    if (file.exists()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  tr("Plik istnieje"),
                                                                  tr("Plik %1 już istnieje.\nCzy chcesz go zastąpić?")
                                                                  .arg(QDir::toNativeSeparators(fileName)),
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Błąd"),
                             tr("Nie można zapisać pliku %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << tr("Odległość,Czas (mm:ss),Milisekundy,Surowy czas (ms)\n");

    QString rawData = display->toPlainText();
    QStringList lines = rawData.split('\n');
    QRegularExpression regex(regexPattern);
    bool dataWritten = false;

    for (const QString &line: lines) {
        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            QString distance = match.captured(1);
            int timeMs = match.captured(2).toInt();

            int minutes = timeMs / 60000;
            int seconds = (timeMs % 60000) / 1000;
            int milliseconds = timeMs % 1000;

            QString timeFormatted = QString("%1:%2")
                    .arg(minutes, 2, 10, QChar('0'))
                    .arg(seconds, 2, 10, QChar('0'));

            out << QString("%1,%2,%3,%4\n")
                    .arg(distance)
                    .arg(timeFormatted)
                    .arg(milliseconds)
                    .arg(timeMs);

            dataWritten = true;
        }
    }

    file.close();

    if (dataWritten) {
        QMessageBox::information(this, tr("Sukces"),
                                 tr("Dane zostały zapisane w %1")
                                 .arg(QDir::toNativeSeparators(fileName)));
    }
}

void MainWindow::updateStopwatch1() {
    stopwatchTime1 += 10;
    int mins = stopwatchTime1 / 60000;
    int secs = (stopwatchTime1 % 60000) / 1000;
    int ms = stopwatchTime1 % 1000;

    stopwatchLabel1->setText(tr("Stoper: %1:%2.%3")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0')));
}

void MainWindow::updateStopwatch2() {
    stopwatchTime2 += 10;
    int mins = stopwatchTime2 / 60000;
    int secs = (stopwatchTime2 % 60000) / 1000;
    int ms = stopwatchTime2 % 1000;

    stopwatchLabel2->setText(tr("Stoper: %1:%2.%3")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0')));
}

void MainWindow::resetStopwatch1() {
    stopwatchTime1 = 0;
    stopwatchRunning1 = false;
    stopwatchTimer1->stop();
    stopwatchLabel1->setText(tr("Stoper: 00:00.000"));
}

void MainWindow::resetStopwatch2() {
    stopwatchTime2 = 0;
    stopwatchRunning2 = false;
    stopwatchTimer2->stop();
    stopwatchLabel2->setText(tr("Stoper: 00:00.000"));
}

void MainWindow::onSensitivityChanged(int value) {
    dropSensitivity = value;
    sensitivityLabel->setText(QString::number(value));
}

void MainWindow::startReading() {
    resetStopwatch1();
    resetStopwatch2();
    lastDistance1 = 0;
    lastDistance2 = 0;

    dataBuffer1.clear();
    dataBuffer2.clear();

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

    // Otwieraj drugi port tylko jeśli checkbox jest zaznaczony
    if (enableSecondLidarCheckBox && enableSecondLidarCheckBox->isChecked()) {
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
            delete serialPort2;
            serialPort2 = nullptr;
        } else {
            connect(serialPort2, &QSerialPort::readyRead, this, [this]() {
                QByteArray data = serialPort2->readAll();
                dataBuffer2.append(QString::fromUtf8(data));
                processBuffer(dataBuffer2, dataDisplay2, &MainWindow::parseData2);
            });
        }
    }
}

void MainWindow::processBuffer(QString &buffer, QTextEdit *display, void (MainWindow::*parseFunc)(const QString &)) {
    static QRegularExpression completePattern("YY\\d+T\\d+E");

    QString processedData;
    int lastMatchEnd = 0;

    QRegularExpressionMatchIterator matches = completePattern.globalMatch(buffer);

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString completeMatch = match.captured(0);

        processedData += completeMatch + "\n";

        lastMatchEnd = match.capturedEnd();
    }

    if (!processedData.isEmpty()) {
        display->append(processedData);


        (this->*parseFunc)(processedData);


        buffer = buffer.mid(lastMatchEnd);
    }
}

void MainWindow::parseData(const QString &data) {
    static QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(data);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        int newDistance = match.captured(1).toInt();
        if (newDistance > 1000) {
            newDistance = newDistance / 10;
        }
        int newTime = match.captured(2).toInt();

        if (newDistance == 0 && distance > 0) {
            continue;
        }


        distance = newDistance;
        timeInMilliseconds = newTime;


        minutes = timeInMilliseconds / 60000;
        seconds = (timeInMilliseconds % 60000) / 1000;
        milliseconds = timeInMilliseconds % 1000;


        dataPoints.append({distance, timeInMilliseconds});
        distanceInput->setText(QString::number(distance));

        updateGlobalTimeDisplay(timeInMilliseconds);


        if (stoper1Enabled) {
            checkForDrop1(distance);
        }
    }
}

void MainWindow::parseData2(const QString &data) {
    static QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatchIterator matchIterator = regex.globalMatch(data);

    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();

        int newDistance = match.captured(1).toInt();
        if (newDistance > 1000) {
            newDistance = newDistance / 10;
        }
        int newTime = match.captured(2).toInt();


        if (newDistance == 0 && distance2 > 0) {
            continue;
        }


        distance2 = newDistance;
        timeInMilliseconds2 = newTime;
        minutes2 = timeInMilliseconds2 / 60000;
        seconds2 = (timeInMilliseconds2 % 60000) / 1000;
        milliseconds2 = timeInMilliseconds2 % 1000;
        dataPoints2.append({distance2, timeInMilliseconds2});
        distanceInput2->setText(QString::number(distance2));

        updateGlobalTimeDisplay(timeInMilliseconds2);

        if (stoper2Enabled) {
            checkForDrop2(distance2);
        }
    }
}
void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
        // Propagacja do okien podrzędnych
        if (portSettings) portSettings->retranslateUi();
        if (appMenu) appMenu->retranslateUi();
        // Jeśli masz wskaźnik do graphWindow, dodaj:
        // if (graphWindow) graphWindow->retranslateUi();
    }
    QMainWindow::changeEvent(event);
}
void MainWindow::stopReading() {
    Reading = false;


    if (stopwatchRunning1) {
        stopwatchRunning1 = false;
        stopwatchTimer1->stop();
    }
    if (stopwatchRunning2) {
        stopwatchRunning2 = false;
        stopwatchTimer2->stop();
    }


    if (serialPort && serialPort->isOpen()) {
        disconnect(serialPort, &QSerialPort::readyRead, nullptr, nullptr);
        serialPort->close();
        qDebug() << tr("Stopped reading from port 1");
        delete serialPort;
        serialPort = nullptr;
    }


    if (serialPort2 && serialPort2->isOpen()) {
        disconnect(serialPort2, &QSerialPort::readyRead, nullptr, nullptr);
        serialPort2->close();
        qDebug() << tr("Stopped reading from port 2");
        delete serialPort2;
        serialPort2 = nullptr;
    }
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_F7) {
        bool visible = !dataDisplay->isVisible();
        dataDisplay->setVisible(visible);
        dataDisplay2->setVisible(visible);
        showRawDataBtn->setText(visible ? tr("Ukryj surowe dane") : tr("Pokaż surowe dane"));
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::showAboutUsDialog() {
    auto *dialog = new AboutUsDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::logDropEvent(int sensorId, int previousDistance, int currentDistance, int difference) {
    QTextEdit *display = (sensorId == 1) ? dataDisplay : dataDisplay2;
    QDateTime currentTime = QDateTime::currentDateTime();


    display->append(tr("[%1] Wykryto spadek (sensor %2): Poprzednia odległość: %3, Aktualna: %4, Różnica: %5")
                   .arg(currentTime.toString("hh:mm:ss.zzz"))
                   .arg(sensorId)
                   .arg(previousDistance)
                   .arg(currentDistance)
                   .arg(difference));


    QScrollBar *scrollBar = display->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());

    qDebug() << "Wykryto spadek na sensorze" << sensorId
             << "- Poprzednia:" << previousDistance
             << "Aktualna:" << currentDistance
             << "Różnica:" << difference;
}

void MainWindow::updateGlobalTimeDisplay(int time) {

    int mins = time / 60000;
    int secs = (time % 60000) / 1000;
    int ms = time % 1000;


    globalTimeLabel->setText(QString("%1:%2.%3")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0')));
}

void MainWindow::onEnableSecondLidarToggled(bool checked) {
    if (isReading) {
        if (checked) {
            // Użytkownik włączył drugi lidar podczas pomiaru
            if (!serialPort2) {
                QString portName2 = portSettings->getPortName2();
                int baudRate2 = portSettings->getBaudRate2();
                int dataBits2 = portSettings->getDataBits2();
                int stopBits2 = portSettings->getStopBits2();
                int parity2 = portSettings->getParity2();
                int flowControl2 = portSettings->getFlowControl2();

                serialPort2 = new QSerialPort(this);
                serialPort2->setPortName(portName2);
                serialPort2->setBaudRate(baudRate2);
                serialPort2->setDataBits(static_cast<QSerialPort::DataBits>(dataBits2));
                serialPort2->setStopBits(static_cast<QSerialPort::StopBits>(stopBits2));
                serialPort2->setParity(static_cast<QSerialPort::Parity>(parity2));
                serialPort2->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl2));

                if (!serialPort2->open(QIODevice::ReadOnly)) {
                    QMessageBox::warning(this, tr("Error"),
                                         tr("Failed to open port %1: %2").arg(portName2, serialPort2->errorString()));
                    delete serialPort2;
                    serialPort2 = nullptr;
                } else {
                    connect(serialPort2, &QSerialPort::readyRead, this, [this]() {
                        QByteArray data = serialPort2->readAll();
                        dataBuffer2.append(QString::fromUtf8(data));
                        processBuffer(dataBuffer2, dataDisplay2, &MainWindow::parseData2);
                    });
                }
            }
        } else {
            // Użytkownik wyłączył drugi lidar podczas pomiaru
            if (serialPort2 && serialPort2->isOpen()) {
                disconnect(serialPort2, &QSerialPort::readyRead, nullptr, nullptr);
                serialPort2->close();
                delete serialPort2;
                serialPort2 = nullptr;
            }
        }
    }
}

void MainWindow::createMenu() {
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Dodanie menu "Plik"
    QMenu *fileMenu = menuBar->addMenu(tr("Plik"));

    QAction *exitAction = new QAction(tr("Zakończ"), this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    // Dodanie menu "Edycja"
    QMenu *editMenu = menuBar->addMenu(tr("Edycja"));

    QAction *copyAction = new QAction(tr("Kopiuj"), this);
    connect(copyAction, &QAction::triggered, this, [this]() {
        QApplication::clipboard()->setText(dataDisplay->toPlainText());
    });
    editMenu->addAction(copyAction);

    QAction *pasteAction = new QAction(tr("Wklej"), this);
    connect(pasteAction, &QAction::triggered, this, [this]() {
        dataDisplay->setPlainText(QApplication::clipboard()->text());
    });
    editMenu->addAction(pasteAction);

    // Dodanie menu "Widok"
    QMenu *viewMenu = menuBar->addMenu(tr("Widok"));

    QAction *zoomInAction = new QAction(tr("Powiększ"), this);
    connect(zoomInAction, &QAction::triggered, this, [this]() {
        QFont font = dataDisplay->font();
        font.setPointSize(font.pointSize() + 1);
        dataDisplay->setFont(font);
    });
    viewMenu->addAction(zoomInAction);

    QAction *zoomOutAction = new QAction(tr("Pomniejsz"), this);
    connect(zoomOutAction, &QAction::triggered, this, [this]() {
        QFont font = dataDisplay->font();
        font.setPointSize(font.pointSize() - 1);
        dataDisplay->setFont(font);
    });
    viewMenu->addAction(zoomOutAction);

    // Dodanie menu języka
    QMenu *languageMenu = menuBar->addMenu(tr("Język"));

    languageActionPL = new QAction(tr("Polski"), this);
    languageActionPL->setCheckable(true);
    languageActionPL->setChecked(true);

    languageActionEN = new QAction(tr("English"), this);
    languageActionEN->setCheckable(true);

    QActionGroup *languageGroup = new QActionGroup(this);
    languageGroup->addAction(languageActionPL);
    languageGroup->addAction(languageActionEN);

    languageMenu->addAction(languageActionPL);
    languageMenu->addAction(languageActionEN);

    connect(languageActionPL, &QAction::triggered, this, [this]() { changeLanguage("pl"); });
    connect(languageActionEN, &QAction::triggered, this, [this]() { changeLanguage("en"); });
}

void MainWindow::changeLanguage(const QString &language) {
    QSettings settings;
    settings.setValue("language", language);

    // Usuń stary translator jeśli istnieje
    if (currentTranslator) {
        qApp->removeTranslator(currentTranslator);
        delete currentTranslator;
        currentTranslator = nullptr;
    }

    // Utwórz nowy translator
    currentTranslator = new QTranslator(this);
    bool loaded = false;

    // Spróbuj załadować z zasobów
    QString resourcePath = QString(":/translations/lidar_%1").arg(language);
    if (currentTranslator->load(resourcePath)) {
        loaded = true;
        qDebug() << "Załadowano tłumaczenie z zasobów:" << resourcePath;
    }

    if (loaded) {
        qApp->installTranslator(currentTranslator);
        qDebug() << "Zainstalowano translator dla języka:" << language;
    }

    // Wymuś aktualizację interfejsu
    QEvent languageChangeEvent(QEvent::LanguageChange);
    QCoreApplication::sendEvent(this, &languageChangeEvent);
}

void MainWindow::retranslateUi() {
    setWindowTitle(tr("EMPE - System Pomiarowy"));

    if (portSettingsBtn) portSettingsBtn->setText(tr("Ustawienia portu"));
    if (showGraphBtn) showGraphBtn->setText(tr("Pokaż wykres"));
    if (startStopBtn) {
        startStopBtn->setText(isReading ? tr("STOP") : tr("START"));
    }
    if (saveDataBtn) saveDataBtn->setText(tr("Zapisz dane 1"));
    if (saveData2Btn) saveData2Btn->setText(tr("Zapisz dane 2"));
    if (clearGraphBtn) clearGraphBtn->setText(tr("Wyczyść wykres"));
    if (showRawDataBtn) {
        bool isVisible = dataDisplay && dataDisplay->isVisible();
        showRawDataBtn->setText(isVisible ? tr("Ukryj surowe dane") : tr("Pokaż surowe dane"));
    }
    if (alwaysOnTopCheckbox) alwaysOnTopCheckbox->setText(tr("Zawsze na wierzchu"));
    if (enableSecondLidarCheckBox) enableSecondLidarCheckBox->setText(tr("Włącz drugi lidar"));
    if (creatorsNoteLabel) {
        creatorsNoteLabel->setText(tr("Program powstał w ramach projektu Embodying Math&Physics Education 2023-1-PL01-KA210-SCH-000165829"));
    }

    // Zaktualizuj elementy stopera
    updateStoperLabels();
}

void MainWindow::updateStoperLabels() {
    if (dropCounter1Label) dropCounter1Label->setText(tr("Spadki: %1").arg(dropCount1));
    if (dropCounter2Label) dropCounter2Label->setText(tr("Spadki: %1").arg(dropCount2));
    if (resetStoperBtn) resetStoperBtn->setText(tr("Resetuj"));
    if (enableStoper1CheckBox) enableStoper1CheckBox->setText(tr("Włącz czujnik 1"));
    if (enableStoper2CheckBox) enableStoper2CheckBox->setText(tr("Włącz czujnik 2"));
    if (stoperGroupBox) stoperGroupBox->setTitle(tr("Stoper"));

    // Zaktualizuj etykiety czasu
    updateStopwatch1();
    updateStopwatch2();
}