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
 * Ostatnia Modyfikacja: 3 Lipca 2025
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
#include "debugwindow.h"
#include "stopperswindow.h"
#include "portconfig.h"

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      isReading(false),
      portSettings(new PortSettings(this)),
      Reading(false),
      distance(0), timeInMilliseconds(0), minutes(0), seconds(0), milliseconds(0),
      distance2(0), timeInMilliseconds2(0), minutes2(0), seconds2(0), milliseconds2(0),
      stoppersWindow(nullptr) {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setWindowTitle(tr("EMPE"));
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(5);

    // Dodanie selektora trybu COM na samej górze interfejsu
    createComModeSelector();

    appMenu = new AppMenu(this, this);
    createControls();

    QWidget *dataContainer = new QWidget(this);
    QVBoxLayout *dataLayout = new QVBoxLayout(dataContainer);
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
        openGraphWindow();
    });
    connect(appMenu, &AppMenu::portSettingsRequested, this, [this]() {
        portSettings->exec();
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
    updateUIForComMode(PortConfig::useOneCOM());

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);

    creatorsNoteLabel = new QLabel(tr("Embodying Math&Physics Education project 2023-1-PL01-KA210-SCH-000165829"),
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

void MainWindow::createComModeSelector() {
    comModeBox = new QGroupBox(tr("COM Mode"), this);
    QHBoxLayout* comModeLayout = new QHBoxLayout(comModeBox);

    oneComRadio = new QRadioButton(tr("Single COM"), comModeBox);
    twoComRadio = new QRadioButton(tr("Dual COM"), comModeBox);
    comModeGroup = new QButtonGroup(this);
    comModeGroup->addButton(oneComRadio, 1);
    comModeGroup->addButton(twoComRadio, 2);

    comModeLayout->addWidget(oneComRadio);
    comModeLayout->addWidget(twoComRadio);

    // Domyślnie wybieramy jeden COM
    oneComRadio->setChecked(PortConfig::useOneCOM());
    twoComRadio->setChecked(!PortConfig::useOneCOM());

    // Podłączenie sygnału - poprawiona składnia dla Qt 6
    connect(comModeGroup, &QButtonGroup::idClicked,
            this, &MainWindow::onComModeChanged);

    mainLayout->addWidget(comModeBox);

    // Aktualizacja interfejsu zgodnie z aktualnym trybem
    updateUIForComMode(PortConfig::useOneCOM());
}

void MainWindow::onComModeChanged(int id) {
    bool useOneCom = (id == 1);

    // Zapisanie ustawienia
    PortConfig::setUseOneCOM(useOneCom);

    // Jeśli pomiar jest uruchomiony, zatrzymaj go
    if (Reading) {
        stopReading();
        startStopBtn->setText(tr("Start"));
        isReading = false;
    }

    // Aktualizacja interfejsu
    updateUIForComMode(useOneCom);
}

void MainWindow::updateUIForComMode(bool useOneCom) {
    // Aktualizacja widoczności elementów interfejsu związanych z drugim portem
    if (sensor2Box) {
        sensor2Box->setVisible(!useOneCom);
    }

    if (saveData2Btn) {
        saveData2Btn->setVisible(!useOneCom);
    }

    // Jeśli używamy PortSettings, ukryj drugą zakładkę gdy w trybie jednego portu
    if (portSettings) {
        portSettings->setPort2Visible(!useOneCom);
    }


}

void MainWindow::createControls() {
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    portSettingsBtn = new QPushButton(tr("PORT settings"));
    showGraphBtn = new QPushButton(tr("Show GRAPH"));
    startStopBtn = new QPushButton(tr("START"));
    saveDataBtn = new QPushButton(tr("SAVE data 1"));
    saveData2Btn = new QPushButton(tr("SAVE data 2"));
    clearGraphBtn = new QPushButton(tr("Clear GRAPH"));
    showRawDataBtn = new QPushButton(tr("Show raw data"));
    showRawDataBtn->hide();
    stoppersButton = new QPushButton(tr("Stoppers"));

    buttonLayout->addWidget(portSettingsBtn);
    buttonLayout->addWidget(showGraphBtn);
    buttonLayout->addWidget(startStopBtn);
    buttonLayout->addWidget(saveDataBtn);
    buttonLayout->addWidget(saveData2Btn);
    buttonLayout->addWidget(clearGraphBtn);
    buttonLayout->addWidget(showRawDataBtn);
    buttonLayout->addWidget(stoppersButton);

    mainLayout->addLayout(buttonLayout);

    connect(portSettingsBtn, &QPushButton::clicked, this, [this]() {
        portSettings->exec();
    });

    connect(showGraphBtn, &QPushButton::clicked, this, [this]() {
        openGraphWindow();
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
        showRawDataBtn->setText(visible ? tr("Hide raw data") : tr("Show raw data"));

        if (!visible) {
            setMinimumSize(0, 0);
            resize(600, 200);
            setMinimumSize(minimumSizeHint());
        } else {
            adjustSize();
        }
    });

    connect(stoppersButton, &QPushButton::clicked, this, &MainWindow::openStoppersWindow);

    QGridLayout *controlsLayout = new QGridLayout();


    QGroupBox *sensor1Box = new QGroupBox(tr("Sensor 1"));
    QVBoxLayout *sensor1Layout = new QVBoxLayout(sensor1Box);

    QLabel *distanceLabel = new QLabel(tr("Distance:"));
    distanceInput = new QLineEdit("00");
    distanceInput->setReadOnly(true);

    sensor1Layout->addWidget(distanceLabel);
    sensor1Layout->addWidget(distanceInput);

    QLabel *timeLabel = new QLabel(tr("Time:"));
    timeInput = new QLineEdit("00:00.000");
    timeInput->setReadOnly(true);

    sensor1Layout->addWidget(timeLabel);
    sensor1Layout->addWidget(timeInput);

    sensor2Box = new QGroupBox(tr("Sensor 2"));
    QVBoxLayout *sensor2Layout = new QVBoxLayout(sensor2Box);

    QLabel *distanceLabel2 = new QLabel(tr("Distance:"));
    distanceInput2 = new QLineEdit("00");
    distanceInput2->setReadOnly(true);

    sensor2Layout->addWidget(distanceLabel2);
    sensor2Layout->addWidget(distanceInput2);

    QLabel *timeLabel2 = new QLabel(tr("Time:"));
    timeInput2 = new QLineEdit("00:00.000");
    timeInput2->setReadOnly(true);

    sensor2Layout->addWidget(timeLabel2);
    sensor2Layout->addWidget(timeInput2);

    // Store labels in member variables
    this->timeLabel = timeLabel;
    this->timeLabel2 = timeLabel2;

    globalTimeLabel = new QLabel("00:00.000");
    globalTimeLabel->setAlignment(Qt::AlignCenter);
    globalTimeLabel->hide();
    QFont timeFont = globalTimeLabel->font();
    timeFont.setPointSize(timeFont.pointSize() + 2);
    globalTimeLabel->setFont(timeFont);

    // timeLayout->addWidget(globalTimeLabel);

    controlsLayout->addWidget(sensor1Box, 0, 0);
    controlsLayout->addWidget(sensor2Box, 0, 1);
    // controlsLayout->addWidget(timeBox, 1, 0, 1, 2);

    mainLayout->addLayout(controlsLayout);

    alwaysOnTopCheckbox = new QCheckBox(tr("Always on Top"), this);
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

void MainWindow::openStoppersWindow() {
    if (!stoppersWindow) {
        stoppersWindow = new StoppersWindow(this);
    }
    stoppersWindow->show();
    stoppersWindow->raise();
    stoppersWindow->activateWindow();
}

void MainWindow::saveDataToFile(const QTextEdit *display, const QString &regexPattern) {
    QString defaultFileName = QString("EMPE_%1.csv").arg(QDate::currentDate().toString("ddMMyyyy"));
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Data"), defaultFileName,
                                                    tr("CSV Files (*.csv)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        fileName += ".csv";
    }

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
    out << tr("Distance,Time (mm:ss),Milliseconds,Raw Time (ms)\n");

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
        QMessageBox::information(this, tr("Success"),
                                 tr("Data has been saved to %1")
                                 .arg(QDir::toNativeSeparators(fileName)));
    }
}

void MainWindow::handleStartStopButton() {
    if (isReading) {
        stopReading();
        startStopBtn->setText(tr("Start"));
    } else {
        startReading();
        startStopBtn->setText(tr("Stop"));
    }
    isReading = !isReading;
}

void MainWindow::retranslateUi() {
    setWindowTitle(tr("EMPE"));

    // Update button texts
    portSettingsBtn->setText(tr("Port settings"));
    showGraphBtn->setText(tr("Show Graph"));
    startStopBtn->setText(isReading ? tr("Stop") : tr("Start"));
    saveDataBtn->setText(tr("Save data 1"));
    saveData2Btn->setText(tr("Save data 2"));
    clearGraphBtn->setText(tr("Clear Graph"));
    showRawDataBtn->setText(dataDisplay->isVisible() ? tr("Hide raw data") : tr("Show raw data"));
    stoppersButton->setText(tr("Stoppers"));

    // Update other labels
    if (timeLabel) timeLabel->setText(tr("Time:"));
    if (timeLabel2) timeLabel2->setText(tr("Time:"));
    alwaysOnTopCheckbox->setText(tr("Always on Top"));
    creatorsNoteLabel->setText(tr("Embodying Math&Physics Education project 2023-1-PL01-KA210-SCH-000165829"));

    if (appMenu) {
        appMenu->updateStartStopAction(isReading);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    // Ctrl+0 combination to open DEBUG MENU
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_0) {
        openDebugWindow();
    }
}

void MainWindow::showAboutUsDialog() {
    auto *dialog = new AboutUsDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::openDebugWindow() {
    if (!debugWindow) {
        debugWindow = new DebugWindow(this);
        debugWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(debugWindow, &QObject::destroyed, this, [this]() { debugWindow = nullptr; });
    }
    debugWindow->show();
    debugWindow->raise();
    debugWindow->activateWindow();
}
void MainWindow::openGraphWindow() {
    if (!graphWindow) {
        graphWindow = new GraphWindow(this);
        graphWindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(graphWindow, &QObject::destroyed, this, [this]() { graphWindow = nullptr; });
    }
    graphWindow->show();
    graphWindow->raise();
    graphWindow->activateWindow();
}
// Methods to accept fake data from DebugWindow
void MainWindow::fakeData1(const QString &data) {
    // process single fake data string
    parseData(data);
}

void MainWindow::fakeData2(const QString &data) {
    parseData2(data);
}

void MainWindow::startReading() {
    if (!portSettings) {
        return;
    }

    // Konfiguracja dla pierwszego portu
    auto config1 = portSettings->getPortConfig(1);
    QString portName1 = config1.portName;
    int baudRate1 = config1.baudRate;
    int dataBits1 = config1.dataBits;
    int stopBits1 = config1.stopBits;
    int parity1 = config1.parity;
    int flowControl1 = config1.flowControl;

    // Konfiguracja portu szeregowego 1
    serialPort1 = new QSerialPort(this);
    serialPort1->setPortName(portName1);
    serialPort1->setBaudRate(baudRate1);
    serialPort1->setDataBits(static_cast<QSerialPort::DataBits>(dataBits1));
    serialPort1->setStopBits(static_cast<QSerialPort::StopBits>(stopBits1));
    serialPort1->setParity(static_cast<QSerialPort::Parity>(parity1));
    serialPort1->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControl1));

    if (!serialPort1->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open port" << portName1 << "Error:" << serialPort1->errorString();
        QMessageBox::warning(this, tr("Error"),
                            tr("Failed to open port %1: %2").arg(portName1, serialPort1->errorString()));
        delete serialPort1;
        serialPort1 = nullptr;
        return;
    }

    // Sprawdź, czy używamy jednego czy dwóch portów COM
    bool useOneCOM = PortConfig::useOneCOM();

    // Konfiguracja dla drugiego portu tylko jeśli używamy dwóch portów
    if (!useOneCOM) {
        auto config2 = portSettings->getPortConfig(2);
        QString portName2 = config2.portName;
        int baudRate2 = config2.baudRate;
        int dataBits2 = config2.dataBits;
        int stopBits2 = config2.stopBits;
        int parity2 = config2.parity;
        int flowControl2 = config2.flowControl;

        // Konfiguracja portu szeregowego 2
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

            if (serialPort1 && serialPort1->isOpen()) {
                serialPort1->close();
                delete serialPort1;
                serialPort1 = nullptr;
            }

            delete serialPort2;
            serialPort2 = nullptr;
            return;
        }

        qDebug() << "Started reading from both ports";
    } else {
        qDebug() << "Started reading from port 1 only";
    }

    Reading = true;

    // Używamy lokalnych wskaźników dla lambda wyrażeń
    QTimer::singleShot(50, this, [this, useOneCOM]() {
        // Podłącz pierwszy port
        connect(this->serialPort1, &QSerialPort::readyRead, this, [this]() {
            QByteArray data = this->serialPort1->readAll();
            dataBuffer1.append(QString::fromUtf8(data));
            processBuffer(dataBuffer1, dataDisplay, &MainWindow::parseData);
        });

        // Podłącz drugi port tylko jeśli używamy dwóch portów
        if (!useOneCOM && this->serialPort2) {
            connect(this->serialPort2, &QSerialPort::readyRead, this, [this]() {
                QByteArray data = this->serialPort2->readAll();
                dataBuffer2.append(QString::fromUtf8(data));
                processBuffer(dataBuffer2, dataDisplay2, &MainWindow::parseData2);
            });
        }
    });
}

void MainWindow::stopReading() {
    if (serialPort1 && serialPort1->isOpen()) {
        serialPort1->close();
    }
    delete serialPort1;
    serialPort1 = nullptr;

    if (serialPort2 && serialPort2->isOpen()) {
        serialPort2->close();
    }
    delete serialPort2;
    serialPort2 = nullptr;

    Reading = false;
    if (appMenu) {
        appMenu->updateStartStopAction(false);
    }
}

void MainWindow::parseData(const QString &data) {
    QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatch match = regex.match(data);

    if (match.hasMatch()) {
        distance = match.captured(1).toInt();
        timeInMilliseconds = match.captured(2).toInt();

        minutes = timeInMilliseconds / 60000;
        seconds = (timeInMilliseconds % 60000) / 1000;
        milliseconds = timeInMilliseconds % 1000;

        QString timeStr = QString("%1:%2.%3")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(seconds, 2, 10, QChar('0'))
                .arg(milliseconds, 3, 10, QChar('0'));

        distanceInput->setText(QString::number(distance));
        timeInput->setText(timeStr);

        dataPoints.append({distance, timeInMilliseconds});

        if (stoppersWindow) {
            stoppersWindow->checkForDrop1(distance);
        }
    }
}

void MainWindow::parseData2(const QString &data) {
    QRegularExpression regex("YY(\\d+)T(\\d+)E");
    QRegularExpressionMatch match = regex.match(data);

    if (match.hasMatch()) {
        distance2 = match.captured(1).toInt();
        timeInMilliseconds2 = match.captured(2).toInt();

        minutes2 = timeInMilliseconds2 / 60000;
        seconds2 = (timeInMilliseconds2 % 60000) / 1000;
        milliseconds2 = timeInMilliseconds2 % 1000;

        QString timeStr = QString("%1:%2.%3")
                .arg(minutes2, 2, 10, QChar('0'))
                .arg(seconds2, 2, 10, QChar('0'))
                .arg(milliseconds2, 3, 10, QChar('0'));

        distanceInput2->setText(QString::number(distance2));
        timeInput2->setText(timeStr);

        dataPoints2.append({distance2, timeInMilliseconds2});

        if (stoppersWindow) {
            stoppersWindow->checkForDrop2(distance2);
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


void MainWindow::updateGlobalTimeDisplay(int timeMs) {
    int minutes = timeMs / 60000;
    int seconds = (timeMs % 60000) / 1000;
    int milliseconds = timeMs % 1000;

    QString formattedTime = QString("%1:%2.%3")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
            .arg(milliseconds, 3, 10, QChar('0'));

    if (globalTimeLabel) {
        globalTimeLabel->setText(formattedTime);
    }
}
void MainWindow::closeEvent(QCloseEvent *event) {
    // Zatrzymanie odczytu przed zamknięciem
    if (isReading) {
        stopReading();
        startStopBtn->setText(tr("Start"));
        isReading = false;
    }
    if (debugWindow) debugWindow->close();
    if (stoppersWindow) {
        stoppersWindow->close();
        stoppersWindow = nullptr;
    }
    if (graphWindow) {
        graphWindow->close();
        graphWindow = nullptr;
    }

    QMainWindow::closeEvent(event);
}
bool MainWindow::switchLanguage(const QString &language) {
    // Tworzenie nowego translatora
    static QTranslator *newTranslator = nullptr;

    // Usuwamy poprzedni translator jeśli istnieje
    if (newTranslator) {
        qApp->removeTranslator(newTranslator);
        delete newTranslator;
        newTranslator = nullptr;
    }

    // Tworzymy nowy translator
    newTranslator = new QTranslator(this);

    // Próba załadowania pliku translacji
    bool success = newTranslator->load(":/translations/lidar_" + language);

    if (success) {
        // Zapisz wybór w ustawieniach
        QSettings settings("EMPE", "LidarApp");
        settings.setValue("language", language);

        // Instalacja nowego translatora
        qApp->installTranslator(newTranslator);

        // Aktualizacja interfejsu
        QEvent *event = new QEvent(QEvent::LanguageChange);
        QCoreApplication::sendEvent(qApp, event);

        // Dodatkowe wywołania metod przetłumaczenia interfejsu
        this->retranslateUi();

        // Aktualizacja interfejsu wszystkich otwartych okien
        foreach(QWidget *widget, QApplication::allWidgets()) {
            QEvent languageEvent(QEvent::LanguageChange);
            QApplication::sendEvent(widget, &languageEvent);
        }

        return true;
    } else {
        delete newTranslator;
        newTranslator = nullptr;
        return false;
    }
}
