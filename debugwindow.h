/*
* Nazwa Projektu: EMPE
 * Plik: debugwindow.h
 *
 * Krótki opis pliku: Deklaracja klasy okna debugowania zawierającego globalny czas i surowe dane.
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

#pragma once

#include <QTextEdit>
#include <QResizeEvent>
#include <QPushButton>
#include "stopperswindow.h"
#include "portconfig.h"  // Dodanie importu dla PortConfig

class MainWindow;
class AppMenu;

class DebugWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit DebugWindow(MainWindow *mainWindow, QWidget *parent = nullptr);

    ~DebugWindow() override = default;

    void updateGlobalTime(int timeInMilliseconds);

    void updateRawData1(const QString &data);

    void updateRawData2(const QString &data);

protected:
    void changeEvent(QEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateDisplays();

    void handleFakeDataButton();

    void onFakeDataTimeout();

    // Nowa metoda do aktualizacji interfejsu po zmianie trybu COM
    void updateUIForComMode(bool useOneCom);

private:
    void retranslateUi();

    void setupUI();

    MainWindow *mainWindow;
    AppMenu *appMenu;

    // UI Elements
    QLabel *titleLabel;
    QLabel *globalTimeLabel;
    QTextEdit *rawDataDisplay1;
    QTextEdit *rawDataDisplay2;
    QLabel *rawDataLabel1;
    QLabel *rawDataLabel2;
    QPushButton *resetStoperBtn;
    QPushButton *fakeDataBtn;

    // Wskaźnik do grupującego boxa dla drugiego sensora
    QGroupBox *rawData2GroupBox;

    // Timer for periodic updates
    QTimer *updateTimer;
    QTimer *fakeDataTimer;
    int fakeTime;
    bool fakeRunning = false;
};
