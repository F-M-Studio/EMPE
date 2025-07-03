/*
* Nazwa Projektu: EMPE
 * Plik: mainwindow.h
 *
 * Krótki opis pliku: inicjalizacja i elementy logiki głównego okna aplikacji (nagłówki)
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMenu>
#include <QAction>
#include <QSerialPort>
#include <QtWidgets>
#include <QDateTime>

#include "portsettings.h"

class PortSettings;
class AppMenu;
class MovingAverageFilterParallel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    bool Reading;
    int distance, timeInMilliseconds, minutes, seconds, milliseconds;
    int distance2, timeInMilliseconds2, minutes2, seconds2, milliseconds2;

    void handleStartStopButton();

    void saveDataToFile(const QTextEdit *display, const QString &regex);

    QTextEdit *dataDisplay;
    QTextEdit *dataDisplay2;

    struct DataPoint {
        int distance;
        int timeInMilliseconds;
    };

    QVector<DataPoint> dataPoints;
    QVector<DataPoint> dataPoints2;

    struct DropEvent {
        QDateTime timestamp;
        int previousValue;
        int currentValue;
        int dropAmount;
        int sensorNumber;
    };

    // Metoda do zmiany języka w czasie rzeczywistym
    bool switchLanguage(const QString &language);
    // Methods for injecting fake data from DebugWindow
    void fakeData1(const QString &data);
    void fakeData2(const QString &data);

    void resetStoperCounters();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void showAboutUsDialog();
    void onSensitivityChanged(int value);
    void saveStoperLogs();

private:
    void openDebugWindow();

    QLabel *timeLabel;
    QLabel *timeLabel2;

    QLabel *globalTimeLabel;
    void updateGlobalTimeDisplay(int time);

    QPushButton *startStopStoperBtn;
    QTimer *stoperTimer;
    int stoperTime;
    bool stoperRunning;

    void handleStoperStartStop();
    void updateStoperTime();
    QDateTime lastDropTime1;
    QDateTime lastDropTime2;
    const int DROP_COOLDOWN_MS = 800;
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QComboBox *portBox{};
    QCheckBox *alwaysOnTopCheckbox{};

    QString dataBuffer1;
    QString dataBuffer2;
    std::vector<double> distanceBuffer;
    MovingAverageFilterParallel *filter = nullptr;

    void createMenu();

    void createControls();

    void startReading();

    void stopReading();

    void parseData(const QString &data);

    void parseData2(const QString &data);

    void processBuffer(QString &buffer, QTextEdit *display, void (MainWindow::*parseFunc)(const QString &));

    AppMenu *appMenu;

    QTimer *validationTimer{};
    bool deviceValidated = false;
    const int VALIDATION_TIMEOUT = 3000;

    QTranslator *translator = nullptr;
    QAction *languageAction{};

    void retranslateUi();


    QMenuBar *menuBar{};
    QMenu *mainMenu{};
    PortSettings *portSettings;
    QAction *portSettingsAction{};
    QAction *graphAction{};
    QAction *startMeasurementAction{};
    QAction *saveDataAction{};

    QPushButton *portSettingsBtn{};
    QPushButton *showGraphBtn{};
    QPushButton *startStopBtn{};
    QPushButton *saveDataBtn{};
    QPushButton *saveData2Btn{};
    QPushButton *clearGraphBtn{};
    QPushButton *showRawDataBtn{};

    QSlider *yAxisSlider{};
    QLineEdit *maxYInput{};
    QLineEdit *distanceInput{};
    QLineEdit *distanceInput2{};
    QLineEdit *timeInput{};
    QLineEdit *timeInput2{};
    QLabel *yAxisValueLabel{};

    QSerialPort *serialPort{};
    QSerialPort *serialPort2{};

    bool isReading;

    QCheckBox *rawDataToggle{};
    bool useRawData = false;
    int lastValidDistance = 0;
    QLabel *creatorsNoteLabel{};


    const int SIGNIFICANT_CHANGE_THRESHOLD = 30;
    const int SMALL_CHANGE_THRESHOLD = 10;


    QTimer *stopwatchTimer1;
    QTimer *stopwatchTimer2;
    int stopwatchTime1 = 0;
    int stopwatchTime2 = 0;
    bool stopwatchRunning1 = false;
    bool stopwatchRunning2 = false;
    QLabel *stopwatchLabel1;
    QLabel *stopwatchLabel2;
    int lastDistance1 = 0;
    int lastDistance2 = 0;


    void updateStopwatch1();
    void updateStopwatch2();
    void resetStopwatch1();
    void resetStopwatch2();

    QCheckBox *stopwatchCheck1;
    QCheckBox *stopwatchCheck2;


    QGroupBox *stoperGroupBox;
    QSlider *sensitivitySlider;
    QLabel *sensitivityLabel;
    QLabel *dropCounter1Label;
    QLabel *dropCounter2Label;
    QPushButton *resetStoperBtn;
    QPushButton *saveStoperLogsBtn;
    QCheckBox *enableStoper1CheckBox;
    QCheckBox *enableStoper2CheckBox;


    int dropSensitivity = 50;
    int dropCount1 = 0;
    int dropCount2 = 0;
    int previousDistance1 = 0;
    int previousDistance2 = 0;
    bool stoper1Enabled = true;
    bool stoper2Enabled = true;


    QVector<DropEvent> dropEvents;


    void createStoperControls();
    void checkForDrop1(int currentDistance);
    void checkForDrop2(int currentDistance);
    void logDropEvent(int sensorId, int previousDistance, int currentDistance, int difference);    void updateStoperDisplay();
};

#endif

