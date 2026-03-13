# README.md

# Relay Endurance Tester

System do długoterminowego testowania przekaźników sterujących silnikiem indukcyjnym.
Tester wykonuje setki tysięcy cykli przełączeń przekaźników oraz umożliwia okresowe pomiary rezystancji styków roboczych.

Sterownik oparty jest na **Arduino Mega 2560** i działa autonomicznie – po zaniku zasilania test zostaje automatycznie wznowiony na podstawie danych zapisanych w EEPROM.

---

# Funkcje systemu

* test trwałości przekaźników do **500 000+ cykli**
* sterowanie **10 kanałami**
* każdy kanał posiada:

  * przekaźnik główny (ON/OFF)
  * przekaźnik kierunku (LEFT/RIGHT)
* sekwencyjne załączanie i wyłączanie kanałów
* automatyczna zmiana kierunku po każdym cyklu
* punkty pomiarowe co określoną liczbę cykli
* tryb TEST do ręcznego pomiaru rezystancji styków
* automatyczne wznowienie testu po zaniku zasilania
* zapis stanu do EEPROM (CRC + double buffer)
* interfejs CLI przez UART
* wyświetlacz LCD 16x2 (I2C)
* watchdog sprzętowy

---

# Sprzęt

Sterownik:

Arduino Mega 2560

Interfejs użytkownika:

LCD 16x2 (I2C)

Sterowanie:

10 modułów przekaźnikowych

Każdy moduł:

relay_i_on_off
relay_i_left_right

czyli:

10 × przekaźnik główny
10 × przekaźnik kierunku

Razem:

20 wyjść cyfrowych.

---

# Zasada działania

Jeden cykl testu składa się z dwóch faz.

## Faza załączania

Kanały włączane są kolejno:

relay_1_on
relay_2_on
relay_3_on
...
relay_10_on

Każde przełączenie następuje co:

STEP_DELAY

## Faza wyłączania

Po załączeniu wszystkich kanałów następuje ich sekwencyjne wyłączenie:

relay_1_off
relay_2_off
...
relay_10_off

Każdy kanał pracuje przez identyczny czas.

Po wyłączeniu ostatniego przekaźnika następuje zmiana kierunku.

LEFT → RIGHT → LEFT → RIGHT

---

# Dead Time

Po wyłączeniu ostatniego przekaźnika głównego wprowadzony jest krótki czas martwy:

DEAD_TIME = 100 ms

Zapewnia to:

* pełne rozwarcie styków
* zanik prądów indukcyjnych
* bezpieczne przełączenie przekaźników kierunku

---

# Interlock bezpieczeństwa

Przekaźnik kierunku **nie może zostać przełączony**, jeśli przekaźnik główny jest załączony.

Warunek:

relay_on_off == OFF

---

# Parametry testu

Domyślne wartości:

STEP_DELAY = 2000 ms
TARGET_CYCLES = 500000
MEASURE_INTERVAL = 20000
SAVE_INTERVAL = 10

Parametry można zmieniać przez UART.

---

# Punkty pomiarowe

Co określoną liczbę cykli system zatrzymuje test i przechodzi do:

STATE_WAIT_MEASUREMENT

W tym stanie wykonywany jest pomiar rezystancji styków.

Test można wznowić komendą:

CONTINUE

---

# Tryb TEST

Tryb TEST umożliwia pomiar rezystancji styków roboczych.

Komenda:

TEST

Powoduje:

* wszystkie przekaźniki kierunku OFF
* wszystkie przekaźniki główne ON

Powrót:

RELEASE

Tryb TEST można uruchomić w stanach:

STATE_IDLE
STATE_WAIT_MEASUREMENT

---

# Autonomiczne wznowienie testu

System zapisuje stan testu w EEPROM.

Zapisywane dane:

cycle_counter
runtime_seconds
direction
power_fail_counter

Po włączeniu zasilania system:

1. odczytuje EEPROM
2. zwiększa licznik restartów
3. wznawia test

chyba że osiągnięto punkt pomiarowy.

---

# EEPROM

Zastosowano:

CRC
double buffer

Struktury:

CONFIG
STATE

Zapisywane dane:

STEP_DELAY
TARGET_CYCLES
MEASURE_INTERVAL
cycle_counter
runtime_seconds
direction
power_fail_counter

---

# Komunikacja UART

Sterowanie odbywa się przez CLI.

## Komendy

PING
STATUS
CONFIG
START
STOP
PAUSE
CONTINUE
RESET
TEST
RELEASE
SET_DELAY <ms>
SET_TARGET <cycles>
SET_INTERVAL <cycles>
HELP

---

# PING

Sprawdzenie komunikacji.

PING
OK

---

# STATUS

Zwraca aktualny stan systemu.

Przykład:

STATE=RUNNING;CYCLES=123456;DIR=RIGHT;TARGET=500000;NEXT_STOP=140000;POWER_FAILS=2;RUNTIME_H=48;FW=1.0

---

# CONFIG

Wyświetla aktualną konfigurację systemu.

CONFIGURATION:
STEP_DELAY=2000
TARGET_CYCLES=500000
MEASURE_INTERVAL=20000
SAVE_INTERVAL=10
RUNTIME_H=48
FW=1.0

---

# Wyświetlacz LCD

LCD pokazuje podstawowe informacje:

linia 1
CYC:123456 DIR:L

linia 2
STATE:RUN

---

# Watchdog

Firmware wykorzystuje watchdog aby zabezpieczyć system przed zawieszeniem podczas wielodniowych testów.

---

# Emergency stop

System posiada fizyczny **Emergency Stop**, który odcina zasilanie przekaźników.

---

# Szacowany czas testu

Przy domyślnych ustawieniach:

STEP_DELAY = 2 s

czas jednego cyklu ≈ 40 s

100 000 cykli ≈ 46 dni
200 000 cykli ≈ 92 dni
500 000 cykli ≈ 231 dni

---

# Dokumentacja

Dokładny opis maszyny stanów znajduje się w pliku:

state_machine.md
