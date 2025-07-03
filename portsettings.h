/*
* Nazwa Projektu: EMPE
 * Plik: portsettings.h
 *
 * Krótki opis pliku: Deklaracja klasy ustawień portów szeregowych umożliwiającej konfigurację połączeń.
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

#ifndef PORTSETTINGS_H
#define PORTSETTINGS_H

#include <QtWidgets>

class PortSettings : public QDialog {
    Q_OBJECT

public:
    struct PortConfig {
        QString portName;
        int baudRate;
        int dataBits;
        int stopBits;
        int parity;
        int flowControl;
    };

    explicit PortSettings(QWidget *parent = nullptr);

    // Zwraca pełną konfigurację dla wybranego portu (1 lub 2)
    PortConfig getPortConfig(int portNumber) const;

private slots:
    void refreshPorts();

    void retranslateUi();

private:
    void updatePortInfo();

    QComboBox *portBox1;
    QComboBox *baudRateBox1;
    QComboBox *dataBitsBox1;
    QComboBox *stopBitsBox1;
    QComboBox *parityBox1;
    QComboBox *flowControlBox1;
    QTextEdit *portInfoText;

    QComboBox *portBox2;
    QComboBox *baudRateBox2;
    QComboBox *dataBitsBox2;
    QComboBox *stopBitsBox2;
    QComboBox *parityBox2;
    QComboBox *flowControlBox2;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *refreshButton;
    QTabWidget *tabWidget;

    void setupUI();

    QWidget *createPortTab(const QString &tabName, QComboBox * &portBox,
                           QComboBox * &baudRateBox, QComboBox * &dataBitsBox,
                           QComboBox * &stopBitsBox, QComboBox * &parityBox,
                           QComboBox * &flowControlBox);

    // Metoda pomocnicza do pobierania konfiguracji z widgetów
    PortConfig getConfigFromWidgets(QComboBox* portBox, QComboBox* baudRateBox,
                                  QComboBox* dataBitsBox, QComboBox* stopBitsBox,
                                  QComboBox* parityBox, QComboBox* flowControlBox) const;

protected:
    void changeEvent(QEvent *event) override;
};
#endif // PORTSETTINGS_H
