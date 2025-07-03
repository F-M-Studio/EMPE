/*
* Nazwa Projektu: EMPE
 * Plik: debugwindow.cpp
 *
 * Krótki opis pliku: Implementacja klasy okna debugowania z globalnym czasem i surowymi danymi.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 3 Lipca 2025
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

#include "debugwindow.h"
#include "mainwindow.h"
#include "appmenu.h"
#include "graphwindow.h"
#include "portsettings.h"
#include <QGroupBox>
#include <QFont>
#include <QKeyEvent>
#include <QScrollBar>
#include <QRandomGenerator>

DebugWindow::DebugWindow(MainWindow *mainWindow, QWidget *parent)
    : QMainWindow(parent), mainWindow(mainWindow) {

    setupUI();
    retranslateUi();

    // Initialize fake data timer
    fakeDataTimer = new QTimer(this);
    fakeDataTimer->setInterval(100);
    connect(fakeDataTimer, &QTimer::timeout, this, &DebugWindow::onFakeDataTimeout);
    fakeTime = 0;

    // Setup update timer
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &DebugWindow::updateDisplays);
    updateTimer->start(50); // Update every 50ms

    // Setup app menu like GraphWindow
    appMenu = new AppMenu(this, mainWindow);
}

void DebugWindow::setupUI() {
    setWindowTitle("DEBUG MENU");
    setMinimumSize(800, 600);
    resize(1000, 700);

    // Central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // Big red title
    titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: red; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    // Global time section
    QGroupBox *timeGroupBox = new QGroupBox(this);
    QVBoxLayout *timeLayout = new QVBoxLayout(timeGroupBox);

    globalTimeLabel = new QLabel("00:00.000", this);
    globalTimeLabel->setAlignment(Qt::AlignCenter);
    QFont timeFont = globalTimeLabel->font();
    timeFont.setPointSize(18);
    timeFont.setBold(true);
    globalTimeLabel->setFont(timeFont);
    globalTimeLabel->setStyleSheet("background-color: #f0f0f0; border: 2px solid #ccc; padding: 10px; margin: 5px;");

    timeLayout->addWidget(globalTimeLabel);
    mainLayout->addWidget(timeGroupBox);

    // Raw data section
    QHBoxLayout *rawDataLayout = new QHBoxLayout();

    // Raw data 1
    QGroupBox *rawData1GroupBox = new QGroupBox(this);
    QVBoxLayout *rawData1Layout = new QVBoxLayout(rawData1GroupBox);

    rawDataLabel1 = new QLabel(this);
    rawDataLabel1->setAlignment(Qt::AlignCenter);
    QFont labelFont = rawDataLabel1->font();
    labelFont.setBold(true);
    rawDataLabel1->setFont(labelFont);

    rawDataDisplay1 = new QTextEdit(this);
    rawDataDisplay1->setReadOnly(true);
    rawDataDisplay1->setFont(QFont("Courier", 9));
    // Note: QTextEdit doesn't have setMaximumBlockCount, but we can limit content manually

    rawData1Layout->addWidget(rawDataLabel1);
    rawData1Layout->addWidget(rawDataDisplay1);

    // Raw data 2
    QGroupBox *rawData2GroupBox = new QGroupBox(this);
    QVBoxLayout *rawData2Layout = new QVBoxLayout(rawData2GroupBox);

    rawDataLabel2 = new QLabel(this);
    rawDataLabel2->setAlignment(Qt::AlignCenter);
    rawDataLabel2->setFont(labelFont);

    rawDataDisplay2 = new QTextEdit(this);
    rawDataDisplay2->setReadOnly(true);
    rawDataDisplay2->setFont(QFont("Courier", 9));
    // Note: QTextEdit doesn't have setMaximumBlockCount, but we can limit content manually

    rawData2Layout->addWidget(rawDataLabel2);
    rawData2Layout->addWidget(rawDataDisplay2);

    rawDataLayout->addWidget(rawData1GroupBox);
    rawDataLayout->addWidget(rawData2GroupBox);

    mainLayout->addLayout(rawDataLayout, 1); // Give raw data section most space
    // Add reset stopwatches button
    resetStoperBtn = new QPushButton(tr("Reset Stopwatches"), this);
    connect(resetStoperBtn, &QPushButton::clicked, this, [this]() {
        if (mainWindow) mainWindow->resetStoperCounters();
    });
    mainLayout->addWidget(resetStoperBtn);
    // Fake data generator button
    fakeDataBtn = new QPushButton(tr("Start Fake Data"), this);
    connect(fakeDataBtn, &QPushButton::clicked, this, &DebugWindow::handleFakeDataButton);
    mainLayout->addWidget(fakeDataBtn);
}

void DebugWindow::retranslateUi() {
    setWindowTitle(tr("DEBUG MENU"));
    titleLabel->setText(tr("DEBUG MENU"));

    // Find the group boxes and set their titles
    QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
    if (groupBoxes.size() >= 3) {
        groupBoxes[0]->setTitle(tr("Global Time"));
        groupBoxes[1]->setTitle(tr("Raw Data - Sensor 1"));
        groupBoxes[2]->setTitle(tr("Raw Data - Sensor 2"));
    }

    rawDataLabel1->setText(tr("Sensor 1 Data Stream"));
    rawDataLabel2->setText(tr("Sensor 2 Data Stream"));
}

void DebugWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void DebugWindow::keyPressEvent(QKeyEvent *event) {
    // Allow closing with Escape
    if (event->key() == Qt::Key_Escape) {
        close();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void DebugWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

void DebugWindow::updateDisplays() {
    if (!mainWindow) return;

    // Update global time from mainWindow
    int currentTime = qMax(mainWindow->timeInMilliseconds, mainWindow->timeInMilliseconds2);
    updateGlobalTime(currentTime);

    // Update raw data displays with current content
    if (mainWindow->dataDisplay && mainWindow->dataDisplay2) {
        QString newData1 = mainWindow->dataDisplay->toPlainText();
        QString newData2 = mainWindow->dataDisplay2->toPlainText();

        // Only update if there's new data to prevent unnecessary scrolling
        if (rawDataDisplay1->toPlainText() != newData1) {
            rawDataDisplay1->setPlainText(newData1);
            // Auto-scroll to bottom
            QScrollBar *scrollBar1 = rawDataDisplay1->verticalScrollBar();
            scrollBar1->setValue(scrollBar1->maximum());
        }

        if (rawDataDisplay2->toPlainText() != newData2) {
            rawDataDisplay2->setPlainText(newData2);
            // Auto-scroll to bottom
            QScrollBar *scrollBar2 = rawDataDisplay2->verticalScrollBar();
            scrollBar2->setValue(scrollBar2->maximum());
        }
    }
}

void DebugWindow::updateGlobalTime(int timeInMilliseconds) {
    int mins = timeInMilliseconds / 60000;
    int secs = (timeInMilliseconds % 60000) / 1000;
    int ms = timeInMilliseconds % 1000;

    globalTimeLabel->setText(QString("%1:%2.%3")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0')));
}

void DebugWindow::updateRawData1(const QString &data) {
    rawDataDisplay1->append(data);
    // Auto-scroll to bottom
    QScrollBar *scrollBar = rawDataDisplay1->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void DebugWindow::updateRawData2(const QString &data) {
    rawDataDisplay2->append(data);
    // Auto-scroll to bottom
    QScrollBar *scrollBar = rawDataDisplay2->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

// Slot: handle fake data start/stop
void DebugWindow::handleFakeDataButton() {
    if (!fakeRunning) {
        fakeRunning = true;
        fakeTime = 0;
        fakeDataTimer->start();
        fakeDataBtn->setText(tr("Stop Fake Data"));
    } else {
        fakeRunning = false;
        fakeDataTimer->stop();
        fakeDataBtn->setText(tr("Start Fake Data"));
    }
}

// Slot: generate fake data on timeout
void DebugWindow::onFakeDataTimeout() {
    // increment fake time
    fakeTime += 100;
    // random distances
    int d1 = QRandomGenerator::global()->bounded(0, 500);
    int d2 = QRandomGenerator::global()->bounded(0, 500);
    // format messages
    QString msg1 = QString("YY%1T%2E").arg(d1).arg(fakeTime);
    QString msg2 = QString("YY%1T%2E").arg(d2).arg(fakeTime);
    // send to main window parsing
    if (mainWindow) {
        mainWindow->fakeData1(msg1 + '\n');
        mainWindow->fakeData2(msg2 + '\n');
    }
}
