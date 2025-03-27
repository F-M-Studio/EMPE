# EMPE LIDAR Reader

Program do odczytu i wizualizacji danych z dwóch urządzeń LIDAR w czasie rzeczywistym.

## Funkcje

- Jednoczesna obsługa dwóch portów szeregowych 
- Wizualizacja pomiarów w czasie rzeczywistym na wykresie
- Konfiguracja parametrów portów szeregowych:
  - Prędkość transmisji (baud rate)
  - Bity danych
  - Bity stopu
  - Parzystość
  - Kontrola przepływu
- Zaawansowane opcje wykresu:
  - Wygładzanie danych
  - Automatyczne usuwanie starych pomiarów
  - Skalowanie osi Y
  - Wybór skali czasu (względna/bezwzględna)
- Zapis danych pomiarowych do pliku CSV
- Wsparcie dla trybu ciemnego/jasnego
- Wielojęzyczny interfejs (polski/angielski)
- Możliwość włączenia trybu "zawsze na wierzchu"

## Wymagania

- Qt 6
- Kompilator C++ obsługujący C++20
- CMake 3.30 lub nowszy

## Kompilacja

1. Sklonuj repozytorium
2. Utwórz katalog build:
```bash
mkdir build && cd build
