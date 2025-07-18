/*
* Nazwa Projektu: EMPE
 * Plik: appmenu.h
 *
 * Krótki opis pliku: Definicja klasy menu aplikacji, odpowiedzialnej za tworzenie i zarządzanie menu głównym.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 10 Marca 2025
 * Ostatnia Modyfikacja: 2 Lipca 2025
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

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTranslator>

class MainWindow;

class AppMenu : public QObject {
    Q_OBJECT

public:
    explicit AppMenu(QMainWindow *window, MainWindow *mainWindow = nullptr);

    ~AppMenu() override = default;

    [[nodiscard]] QMenu *getMainMenu() const { return mainMenu; }

    void updateStartStopAction(bool isReading) const;

signals:
    void graphWindowRequested();

    void portSettingsRequested();

    void aboutUsRequested();


private slots:
    void switchToEnglish();

    void switchToPolish();

    void promptRestart();

private:
    QMainWindow *window;
    MainWindow *mainWindow;
    QMenu *mainMenu{};
    QActionGroup *languageGroup{};

    QAction *portSettingsAction{};
    QAction *graphAction{};
    QAction *startMeasurementAction{};
    QAction *saveDataAction{};
    QAction *aboutUsAction{};
    QAction *englishAction{};
    QAction *polishAction{};
};
