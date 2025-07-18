Of course. Here is the extended `README.md` file, now including a glossary of terms used within the application.

-----

# EMPE - Dokumentacja Aplikacji

## Spis treści

1.  [Wprowadzenie](https://www.google.com/search?q=%23wprowadzenie)
2.  [Główne Funkcje](https://www.google.com/search?q=%23g%C5%82%C3%B3wne-funkcje)
3.  [Instrukcja Obsługi](https://www.google.com/search?q=%23instrukcja-obs%C5%82ugi)
4.  [Opis Funkcji Okien](https://www.google.com/search?q=%23opis-funkcji-okien)
    - [Główne Okno](https://www.google.com/search?q=%23g%C5%82%C3%B3wne-okno)
    - [Okno Wykresu](https://www.google.com/search?q=%23okno-wykresu)
    - [Okno Stoperów](https://www.google.com/search?q=%23okno-stoper%C3%B3w)
    - [Ustawienia Portów](https://www.google.com/search?q=%23ustawienia-port%C3%B3w)
    - [Menu Debugowania](https://www.google.com/search?q=%23menu-debugowania)
5.  [Technologie](https://www.google.com/search?q=%23technologie)
6.  [Kompilacja Projektu](https://www.google.com/search?q=%23kompilacja-projektu)
7.  [Autorzy](https://www.google.com/search?q=%23autorzy)
8.  [Słownik Pojęć / Glossary](https://www.google.com/search?q=%23s%C5%82ownik-poj%C4%99%C4%87--glossary)
9.  [Licencja](https://www.google.com/search?q=%23licencja)

-----

## Wprowadzenie

**EMPE** to zaawansowana aplikacja desktopowa stworzona w ramach projektu *Embodying Math\&Physics Education*. Służy do akwizycji, wizualizacji i analizy danych w czasie rzeczywistym z czujników odległości Lidar. Program został zaprojektowany z myślą o zastosowaniach edukacyjnych, umożliwiając przeprowadzanie precyzyjnych eksperymentów fizycznych, takich jak pomiar spadku swobodnego.

Aplikacja komunikuje się z czujnikami przez porty szeregowe (COM) i oferuje bogaty zestaw narzędzi do interakcji z danymi.

## Główne Funkcje

- **Obsługa dwóch czujników**: Możliwość pracy z jednym lub dwoma czujnikami Lidar jednocześnie, z dynamicznym przełączaniem trybu.
- **Wizualizacja danych na żywo**: Prezentacja odczytów w czasie rzeczywistym na interaktywnym wykresie.
- **Zaawansowana personalizacja wykresu**: Użytkownik może modyfikować m.in. grubość linii, zakres osi, włączać wygładzanie (uśrednianie kroczące) oraz automatyczne przewijanie.
- **Dynamiczne motywy**: Interfejs automatycznie dostosowuje się do jasnego lub ciemnego motywu systemu operacyjnego, zapewniając komfort pracy.
- **Wielojęzyczność**: Pełne wsparcie dla języka polskiego i angielskiego, z możliwością zmiany w trakcie działania aplikacji.
- **Pomiar interwałów (Stopery)**: Dedykowane okno do mierzenia czasu pomiędzy wykrytymi "spadkami" (nagłymi zmianami odległości), z regulowaną czułością.
- **Zapis danych**: Możliwość eksportu zebranych danych pomiarowych oraz interwałów czasowych do plików w formacie `.csv` w celu dalszej analizy.
- **Narzędzia deweloperskie**: Ukryte menu debugowania (dostępne pod skrótem `Ctrl+0`) wyświetlające surowe dane z portów i globalny czas.

## Instrukcja Obsługi

1.  **Uruchomienie**: Po uruchomieniu aplikacji wyświetli się okno główne.
2.  **Konfiguracja portów**:
    - Przejdź do `☰ Menu` -\> `Ustawienia portu`.
    - W nowym oknie wybierz odpowiednie porty COM dla swoich czujników. Możesz odświeżyć listę dostępnych portów przyciskiem `Odśwież`.
    - Skonfiguruj parametry transmisji (prędkość, bity danych etc.) lub pozostaw wartości domyślne.
3.  **Wybór trybu pracy**:
    - W oknie głównym zaznacz pole `Użyj dwóch portów COM`, jeśli podłączone są dwa czujniki. W przeciwnym razie pozostaw je odznaczone.
4.  **Rozpoczęcie pomiaru**:
    - Naciśnij przycisk `Start` w oknie głównym lub na wykresie, aby rozpocząć zbieranie danych. Przycisk zmieni nazwę na `Stop`.
5.  **Obserwacja i analiza**:
    - Otwórz okno wykresu (`☰ Menu` -\> `Wykres`), aby wizualizować dane.
    - Otwórz okno stoperów (`☰ Menu` -\> `Stopery`), aby mierzyć czasy spadków.
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

### Menu Debugowania

Dostępne pod skrótem `Ctrl+0`.

- **Globalny czas**: Wyświetla wspólny, największy czas z obu czujników.
- **Surowe dane**: Pokazuje nieprzetworzone ramki danych odbierane z portów szeregowych, co jest przydatne do diagnozowania problemów z komunikacją.
- **Generator danych**: Przycisk `Start Fake Data` pozwala na testowanie aplikacji bez podłączonych czujników.

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

## Słownik Pojęć / Glossary

| Termin (Polski) | Term (English) | Opis / Description |
| :--- | :--- | :--- |
| **Port COM** | **COM Port** | Szeregowy port komunikacyjny używany do połączenia komputera z czujnikami Lidar. / A serial communication port used to connect the computer to the Lidar sensors. |
| **Tryb Dual COM** | **Dual COM Mode** | Tryb pracy aplikacji, w którym dane są odczytywane jednocześnie z dwóch czujników podłączonych do dwóch różnych portów COM. / An application mode where data is read simultaneously from two sensors connected to two different COM ports. |
| **Dane Surowe** | **Raw Data** | Nieprzetworzone ramki danych (np. `YY100T5000E`) odbierane bezpośrednio z czujników przed ich interpretacją. Dostępne w Menu Debugowania. / The unprocessed data frames (e.g., `YY100T5000E`) received directly from the sensors before being parsed. Available in the Debug Menu. |
| **Stopery / Timery Spadku** | **Stoppers / Drop Timers** | Funkcja oraz okno służące do pomiaru interwałów czasowych pomiędzy wykrytymi "spadkami". / A feature and a window used to measure time intervals between detected "drops". |
| **Spadek** | **Drop** | Zdarzenie rejestrowane, gdy zmierzona odległość gwałtownie zmniejszy się o wartość większą niż ustawiona "Czułość spadku". / An event registered when the measured distance suddenly decreases by a value greater than the set "Drop Sensitivity". |
| **Czułość Spadku** | **Drop Sensitivity** | Konfigurowalny próg (w mm) w oknie Stoperów, który określa, jak duża musi być zmiana odległości, aby została uznana za "spadek". / A configurable threshold (in mm) in the Stoppers window that defines how large the change in distance must be to be considered a "drop". |
| **Interwały** | **Intervals** | Czas, który upłynął pomiędzy dwoma kolejnymi zarejestrowanymi spadkami. Dane te można zapisać do pliku `.csv`. / The time elapsed between two consecutive registered drops. This data can be saved to a `.csv` file. |
| **Wygładzanie Wykresu** | **Graph Smoothing** | Algorytm (uśrednianie kroczące) stosowany do danych na wykresie w celu redukcji szumów i pokazania gładszej linii trendu. / An algorithm (moving average) applied to the graph data to reduce noise and show a smoother trend line. |
| **Generator Danych Fikcyjnych**| **Fake Data Generator**| Narzędzie w Menu Debugowania, które symuluje dane z czujników, umożliwiając testowanie aplikacji bez fizycznego sprzętu. / A tool in the Debug Menu that simulates sensor data, allowing for application testing without physical hardware. |
| **Automatyczne Przewijanie** | **Auto-scroll** | Funkcja wykresu, która automatycznie przesuwa oś czasu w lewo w miarę napływania nowych danych, utrzymując najnowsze pomiary w widoku. / A graph feature that automatically shifts the time axis to the left as new data arrives, keeping the latest measurements in view. |
| **Limit Punktów** | **Points Limit** | W trybie automatycznego przewijania, to ustawienie określa maksymalną liczbę punktów danych wyświetlanych jednocześnie na wykresie. / In auto-scroll mode, this setting determines the maximum number of data points displayed on the graph at one time. |
| **Czas Względny/Absolutny**| **Relative/Absolute Time**| Ustawienie wykresu. Czas absolutny to całkowity czas od uruchomienia czujnika. Czas względny to czas od rozpoczęcia bieżącej sesji pomiarowej (startuje od zera). / A graph setting. Absolute time is the total time since the sensor started. Relative time is the time since the current measurement session began (starts from zero). |
| **Baud Rate** | **Baud Rate** | Prędkość transmisji danych przez port szeregowy, mierzona w bitach na sekundę. Musi być zgodna z ustawieniami czujnika. / The speed of data transmission over the serial port, measured in bits per second. It must match the sensor's settings. |
| **Kontrola Przepływu** | **Flow Control** | Mechanizm w komunikacji szeregowej zapobiegający przepełnieniu bufora odbiornika przez nadajnik. / A mechanism in serial communication to prevent the sender from overwhelming the receiver's buffer. |

## Licencja

Ten program jest wolnym oprogramowaniem; możesz go rozprowadzać i/lub modyfikować na warunkach **Powszechnej Licencji Publicznej GNU, w wersji 3** tej Licencji lub (według twojego wyboru) dowolnej późniejszej wersji.

Szczegóły znajdują się w pliku `LICENSE`.