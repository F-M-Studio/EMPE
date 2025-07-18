/*
* Nazwa Projektu: EMPE
 * Plik: portconfig.h
 *
 * Krótki opis pliku: Konfiguracja portów COM i ustawienia trybu pracy programu (1 lub 2 porty).
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 4 Lipca 2025
 * Ostatnia Modyfikacja: 4 Lipca 2025
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

#ifndef PORTCONFIG_H
#define PORTCONFIG_H

#include <QSettings>
#include <QString>

class PortConfig {
public:
    static bool useOneCOM() {
        QSettings settings("EMPE", "LidarApp");
        return settings.value("useOneCOM", true).toBool();
    }

    static void setUseOneCOM(bool value) {
        QSettings settings("EMPE", "LidarApp");
        settings.setValue("useOneCOM", value);
    }
};

#endif // PORTCONFIG_H
