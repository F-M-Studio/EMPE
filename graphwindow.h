/*
* Nazwa Projektu: EMPE
 * Plik: graphwindow.h
 *
 * Krótki opis pliku: Deklaracja klasy okna wykresu do wizualizacji pomiarów w czasie rzeczywistym.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 10 Marca 2025
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

#pragma once

#include "mainwindow.h"
#include "portconfig.h"

#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE

namespace Ui {
    class GraphWindow;
}

QT_END_NAMESPACE

class GraphWindow : public QMainWindow {
    Q_OBJECT

public:
    void retranslateUi();

    explicit GraphWindow(MainWindow *mainWindow, QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;

    ~GraphWindow() override;

    [[nodiscard]] QChart *getChart() const { return chart; }

protected:
    void changeEvent(QEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateGraph();

    void clearGraph();

private:
    void updateChartTheme();

    AppMenu *appMenu{};

    bool Gen = false;

    Ui::GraphWindow *ui;
    MainWindow *mainWindow;
    QTimer *updateTimer;
    QLineSeries *series;
    QLineSeries *series2;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    const int MAX_POINTS = 100; // Maximum number of points to show

    QSlider *recordingSlider;
    QLabel *recordingLabel{};
    QLabel *recordingValueLabel{};
    QLabel *recordingTitleLabel;
    QLabel *yAxisTitleLabel;
    QLineEdit *recordingEdit;
    QLineEdit *yAxisEdit;

    QPushButton *clearGraphBtn;
    QPushButton *startStopBtn;

    QCheckBox *yAxisToggle;
    QSlider *yAxisSlider;
    QLabel *yAxisLabel{};
    QLabel *yAxisValueLabel{};
    bool manualYAxisControl = false;
    QCheckBox *autoRemoveToggle;
    QSlider *pointsLimitSlider;
    QLineEdit *pointsLimitEdit;
    QLabel *pointsLimitLabel;
    bool autoRemovePoints = true;
    int pointsLimit = 100;


    QSplineSeries *splineSeries;
    QSplineSeries *splineSeries2;
    QCheckBox *smoothingToggle;
    QSlider *smoothingLevelSlider;
    QLineEdit *smoothingLevelEdit;
    QLabel *smoothingLevelLabel;
    bool useSpline = false;

    void applySmoothing() const;

    QCheckBox *timeAxisToggle;
    bool useAbsoluteTime = false;
    long long initialTime = 0;

    QCheckBox *showSeries1Toggle;
    QCheckBox *showSeries2Toggle;
    bool showSeries1 = true;
    bool showSeries2 = true;
};

#endif // GRAPHWINDOW_H
