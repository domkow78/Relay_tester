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
* zapis stanu do EEPROM (CRC + ring buffer wear leveling + sequence)
* zapis stanu przy każdym punkcie pomiarowym
* interfejs CLI przez UART
* wyświetlacz LCD 16x2 (I2C)
* watchdog sprzętowy (AVR WDT, timeout 2 s)

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

Jeden cykl testu to sekwencja **wave** – w danym momencie załączony jest tylko jeden kanał.

## Sekwencja wave

Kanały przełączane są kolejno:

relay_1_on
(wait STEP_DELAY)
relay_1_off / relay_2_on
(wait STEP_DELAY)
relay_2_off / relay_3_on
...
relay_9_off / relay_10_on
(wait STEP_DELAY)
relay_10_off

Każde przejście następuje co STEP_DELAY (domyślnie 1000 ms).

Po wyłączeniu kanału 10 następuje dead time i zmiana kierunku.

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

STEP_DELAY = 1000 ms
TARGET_CYCLES = 100000
MEASURE_INTERVAL = 20000
SAVE_INTERVAL = 100

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

sequence (numer sekwencji zapisu)
cycle_counter
runtime_seconds
direction
power_fail_counter

Po włączeniu zasilania system:

1. odczytuje EEPROM
2. wybiera slot z najwyższym numerem sequence
3. zwiększa licznik restartów
4. wznawia test

chyba że osiągnięto punkt pomiarowy.

---

# EEPROM

Zastosowano:

CRC (CONFIG i STATE)
ring buffer wear leveling (STATE – 271 slotów)
numer sekwencji (sequence) – gwarantuje wybór najnowszego slotu

Mapa EEPROM:

| Adresy | Zawartość |
|--------|----------|
| 0 – 31 | TestConfig (1 slot) |
| 32 – 4095 | TestState ring buffer (sloty po 19 B) |

Struktury:

CONFIG
STATE

Zapisywane dane:

STEP_DELAY
TARGET_CYCLES
MEASURE_INTERVAL
sequence
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
FACTORY_RESET
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

STATE=RUNNING;CYCLES=123456;DIR=RIGHT;TARGET=100000;POWER_FAILS=2;RUNTIME_H=48;FW=1.0

---

# CONFIG

Wyświetla aktualną konfigurację systemu.

CONFIGURATION:
STEP_DELAY=1000
TARGET_CYCLES=100000
MEASURE_INTERVAL=20000
SAVE_INTERVAL=100
FW=1.0

---

# RESET vs FACTORY_RESET

Komenda **RESET** zeruje tylko liczniki (cycle_counter, runtime, power_fail_counter), ale **zachowuje** konfigurację zapisaną w EEPROM.

Komenda **FACTORY_RESET** zeruje liczniki **oraz** przywraca domyślne wartości konfiguracji z pliku config.h:

FACTORY_RESET
FACTORY RESET OK

Użyj FACTORY_RESET gdy:
* zmienisz wartości domyślne w config.h i chcesz je zastosować
* chcesz całkowicie zresetować urządzenie do ustawień fabrycznych

---

# Wyświetlacz LCD

LCD pokazuje podstawowe informacje:

linia 1
CYCLE:123456

linia 2
Format: [4 znaki stanu][spacja][10 znaków wzorca][spacja] = 16 znaków

ON   1000000000    ← STATE_STEP_ON, kanał 1 załączony
ON   0100000000    ← STATE_STEP_ON, kanał 2 załączony
DEAD 0000000000    ← STATE_WAIT_DEAD_TIME
DIR  0000000000    ← STATE_CHANGE_DIRECTION
MEAS 0000000000    ← STATE_WAIT_MEASUREMENT
IDLE 0000000000    ← STATE_IDLE
END  0000000000    ← STATE_FINISHED

Skróty stanów:

| Stan                    | Skrót |
|-------------------------|-------|
| STATE_INIT              | INIT  |
| STATE_IDLE              | IDLE  |
| STATE_RUNNING           | RUN   |
| STATE_STEP_ON           | ON    |
| STATE_STEP_OFF          | OFF   |
| STATE_WAIT_DEAD_TIME    | DEAD  |
| STATE_CHANGE_DIRECTION  | DIR   |
| STATE_WAIT_MEASUREMENT  | MEAS  |
| STATE_TEST_MODE         | TEST  |
| STATE_FINISHED          | END   |
| STATE_ERROR             | ERR   |

Odświeżanie LCD jest ograniczone do **200 ms** (throttle), aby nie obciążać magistrali I2C.

---

# Watchdog

Watchdog sprzętowy AVR jest aktywny.

Timeout: **2 s**

Jeśli główna pętla `loop()` nie wywoła `wdt_reset()` w ciągu 2 sekund, mikrokontroler zostaje automatycznie zrestartowany.

Watchdog jest wyłączany na czas `setup()` (inicjalizacja EEPROM, LCD), a następnie włączany przed wejściem do `loop()`.

---

# Ochrona EEPROM

AVR ATmega2560 gwarantuje 100 000 zapisów na komórkę.

Zastosowany mechanizm: **ring buffer wear leveling** – każdy zapis trafia do następnego slotu w pierścieniu. Przy starcie skanowane są wszystkie sloty; wybrany jest ten z najwyższym `cycle_counter` i poprawnym CRC.

| Parametr | Wartość |
|---|---|
| Liczba slotów STATE | 271 |
| Zapisów przed zużyciem | 271 × 100 000 = **27 100 000** |
| SAVE_INTERVAL | 100 cykli |
| RUNTIME_SAVE_INTERVAL | 300 s |

Przy teście 500 000 cykli liczba zapisów wynosi ~5 800, co oznacza ponad **4 600 pełnych testów** przed zużyciem.

---

# Emergency stop

System posiada fizyczny **Emergency Stop**, który odcina zasilanie przekaźników.

---

# Szacowany czas testu

Przy domyślnych ustawieniach:

STEP_DELAY = 1 s

czas jednego cyklu ≈ 11 s

100 000 cykli ≈ 13 dni
200 000 cykli ≈ 26 dni
500 000 cykli ≈ 64 dni

---

# Dokumentacja

Dokładny opis maszyny stanów znajduje się w pliku:

state_machine.md
