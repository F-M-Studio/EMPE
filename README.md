# EMPE - Dokumentacja Aplikacji

## Spis treści
1. [Wprowadzenie](#wprowadzenie)
2. [Główne Funkcje](#główne-funkcje)
3. [Instrukcja Obsługi](#instrukcja-obsługi)
4. [Opis Funkcji Okien](#opis-funkcji-okien)
    - [Główne Okno](#główne-okno)
    - [Okno Wykresu](#okno-wykresu)
    - [Okno Stoperów](#okno-stoperów)
    - [Ustawienia Portów](#ustawienia-portów)
5. [Technologie](#technologie)
6. [Kompilacja Projektu](#kompilacja-projektu)
7. [Autorzy](#autorzy)
8. [Licencja](#licencja)

---

## Wprowadzenie

**EMPE** to zaawansowana aplikacja desktopowa stworzona w ramach projektu *Embodying Math&Physics Education*. Służy do akwizycji, wizualizacji i analizy danych w czasie rzeczywistym z czujników odległości Lidar. Program został zaprojektowany z myślą o zastosowaniach edukacyjnych, umożliwiając przeprowadzanie precyzyjnych eksperymentów fizycznych, takich jak pomiar spadku swobodnego.

Aplikacja komunikuje się z czujnikami przez porty szeregowe (COM) i oferuje bogaty zestaw narzędzi do interakcji z danymi.

## Główne Funkcje

- **Obsługa dwóch czujników**: Możliwość pracy z jednym lub dwoma czujnikami Lidar jednocześnie, z dynamicznym przełączaniem trybu.
- **Wizualizacja danych na żywo**: Prezentacja odczytów w czasie rzeczywistym na interaktywnym wykresie.
- **Zaawansowana personalizacja wykresu**: Użytkownik może modyfikować m.in. grubość linii, zakres osi, włączać wygładzanie (uśrednianie kroczące) oraz automatyczne przewijanie.
- **Dynamiczne motywy**: Interfejs automatycznie dostosowuje się do jasnego lub ciemnego motywu systemu operacyjnego, zapewniając komfort pracy.
- **Wielojęzyczność**: Pełne wsparcie dla języka polskiego i angielskiego, z możliwością zmiany w trakcie działania aplikacji.
- **Pomiar interwałów (Stopery)**: Dedykowane okno do mierzenia czasu pomiędzy wykrytymi "spadkami" (nagłymi zmianami odległości), z regulowaną czułością.
- **Zapis danych**: Możliwość eksportu zebranych danych pomiarowych oraz interwałów czasowych do plików w formacie `.csv` w celu dalszej analizy.
  
## Instrukcja Obsługi

1.  **Uruchomienie**: Po uruchomieniu aplikacji wyświetli się okno główne.
2.  **Konfiguracja portów**:
    - Przejdź do `☰ Menu` -> `Ustawienia portu`.
    - W nowym oknie wybierz odpowiednie porty COM dla swoich czujników. Możesz odświeżyć listę dostępnych portów przyciskiem `Odśwież`.
    - Skonfiguruj parametry transmisji (prędkość, bity danych etc.) lub pozostaw wartości domyślne.
3.  **Wybór trybu pracy**:
    - W oknie głównym zaznacz pole `Użyj dwóch portów COM`, jeśli podłączone są dwa czujniki. W przeciwnym razie pozostaw je odznaczone.
4.  **Rozpoczęcie pomiaru**:
    - Naciśnij przycisk `Start` w oknie głównym lub na wykresie, aby rozpocząć zbieranie danych. Przycisk zmieni nazwę na `Stop`.
5.  **Obserwacja i analiza**:
    - Otwórz okno wykresu (`☰ Menu` -> `Wykres`), aby wizualizować dane.
    - Otwórz okno stoperów (`☰ Menu` -> `Stopery`), aby mierzyć czasy spadków.
6.  **Zakończenie i zapis**:
    - Naciśnij `Stop`, aby zakończyć pomiar.
    - Użyj przycisków `Zapisz dane 1` / `Zapisz dane 2` w oknie głównym lub `Zapisz wynik` w oknie stoperów, aby wyeksportować dane do pliku `.csv`.

## Opis Funkcji Okien

### Główne Okno
Centrum sterowania aplikacją.
- **Wyświetlacze**: Pokazują ostatni odczyt dystansu i czasu dla każdego z czujników.
- **Przyciski**: Umożliwiają rozpoczęcie/zatrzymanie pomiaru, zapis danych, otwarcie innych okien.
- **Przełącznik trybu COM**: Pozwala wybrać, czy aplikacja ma nasłuchiwać na jednym, czy na dwóch portach.
- **Zawsze na wierzchu**: Utrzymuje okno aplikacji nad innymi oknami.

### Okno Wykresu
Służy do wizualizacji danych.
- **Wykres**: Dynamicznie rysuje odległość w funkcji czasu.
- **Sterowanie**: Przyciski `Start`/`Stop` i `Wyczyść wykres`.
- **Personalizacja**:
    - `Okres rejestracji`: Częstotliwość odświeżania wykresu.
    - `Zakres osi Y`: Ręczne lub automatyczne skalowanie osi Y.
    - `Automatyczne przewijanie osi X`: Włącza/wyłącza przesuwanie się wykresu i pozwala ustawić limit wyświetlanych punktów.
    - `Wygładź wykres`: Aktywuje algorytm uśredniania kroczącego, aby zniwelować szumy.
    - `Grubość linii`: Pozwala dostosować grubość obu serii danych.
    - `Pokaż serię`: Umożliwia tymczasowe ukrycie jednej z serii danych.

### Okno Stoperów
Narzędzie do analizy zdarzeń.
- **Czułość spadku**: Suwak określający, jak duża zmiana odległości (w mm) jest rejestrowana jako "spadek".
- **Liczniki**: Zliczają liczbę wykrytych spadków dla każdego czujnika.
- **Stopery**: Mierzą całkowity czas trwania sesji pomiarowej.
- **Rejestr interwałów**: Wyświetla czas, jaki upłynął pomiędzy kolejnymi spadkami.
- **Zapis wyników**: Eksportuje zarejestrowane interwały do pliku `.csv`.

### Ustawienia Portów
Konfiguracja połączenia z czujnikami.
- **Wybór portu**: Lista dostępnych portów COM.
- **Parametry transmisji**: Baud rate, bity danych, bity stopu, parzystość, kontrola przepływu.
- **Informacje o porcie**: Wyświetla szczegółowe dane o podłączonych urządzeniach.

## Technologie
- **Język**: C++20
- **Framework**: Qt 6 (moduły: Core, Gui, Widgets, SerialPort, Charts, LinguistTools)
- **System budowania**: CMake

## Kompilacja Projektu
Do skompilowania projektu wymagany jest kompilator C++20, CMake oraz zainstalowana biblioteka Qt 6.

1.  Sklonuj repozytorium.
2.  Utwórz katalog na pliki budowania (np. `build`).
3.  Uruchom CMake, aby wygenerować projekt:
    ```bash
    cmake -B build -S .
    ```
4.  Skompiluj projekt:
    ```bash
    cmake --build build
    ```

## Autorzy
- **Mateusz Korniak** `<mkorniak04@gmail.com>`
- **Mateusz Machowski** `<machowskimateusz51@gmail.com>`
- **Filip Leśnik** `<filip.lesnik170@gmail.com>`

## Licencja
Ten program jest wolnym oprogramowaniem; możesz go rozprowadzać i/lub modyfikować na warunkach **Powszechnej Licencji Publicznej GNU, w wersji 3** tej Licencji lub (według twojego wyboru) dowolnej późniejszej wersji.

Szczegóły znajdują się w pliku `LICENSE`.
