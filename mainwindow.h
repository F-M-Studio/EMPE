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

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTranslator>
#include <QSlider>

class PortSettings;
class AppMenu;
class MovingAverageFilterParallel;
class StoppersWindow;
class QSerialPort;
class QMenu;
class QTimer;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    bool Reading;
    int distance, timeInMilliseconds, minutes, seconds, milliseconds;
    int distance2, timeInMilliseconds2, minutes2, seconds2, milliseconds2;

    void handleStartStopButton();

    void saveDataToFile(const QTextEdit *display, const QString &regex);

    QTextEdit *dataDisplay;
    QTextEdit *dataDisplay2;
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;

    struct DataPoint {
        int distance;
        int timeInMilliseconds;
    };

    QVector<DataPoint> dataPoints;
    QVector<DataPoint> dataPoints2;

    // Metoda do zmiany języka w czasie rzeczywistym
    bool switchLanguage(const QString &language);

    // Methods for injecting fake data from DebugWindow
    void fakeData1(const QString &data);

    void fakeData2(const QString &data);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void showAboutUsDialog();

    void openStoppersWindow();

private:
    void openDebugWindow();


    void createMenu();

    void createControls();

    void startReading();

    void stopReading();

    void parseData(const QString &data);

    void parseData2(const QString &data);

    void processBuffer(QString &buffer, QTextEdit *display, void (MainWindow::*parseFunc)(const QString &));

    AppMenu *appMenu;
    StoppersWindow *stoppersWindow;

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
    QPushButton *stoppersButton{};

    QSlider *yAxisSlider{};
    QLineEdit *maxYInput{};
    QLineEdit *distanceInput{};
    QLineEdit *distanceInput2{};
    QLineEdit *timeInput{};
    QLineEdit *timeInput2{};
    QLabel *yAxisValueLabel{};

    QSerialPort *serialPort{};
    QSerialPort *serialPort1{};
    QSerialPort *serialPort2{};

    bool isReading;

    QCheckBox *rawDataToggle{};
    bool useRawData = false;
    int lastValidDistance = 0;
    QLabel *creatorsNoteLabel{};


    const int SIGNIFICANT_CHANGE_THRESHOLD = 30;
    const int SMALL_CHANGE_THRESHOLD = 10;

    // Add missing member variables
    QLabel *timeLabel{};
    QLabel *timeLabel2{};
    QLabel *globalTimeLabel{};
    QCheckBox *alwaysOnTopCheckbox{};

    QString dataBuffer1;
    QString dataBuffer2;

    void updateGlobalTimeDisplay(int timeMs);

    friend class DebugWindow; // Add this line to give DebugWindow access
};

#endif
