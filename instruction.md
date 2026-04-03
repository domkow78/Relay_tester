# Instrukcja obsługi – Relay Endurance Tester

## Spis treści

1. [Wprowadzenie](#wprowadzenie)
2. [Informacje ogólne](#informacje-ogólne)
3. [Interfejs użytkownika](#interfejs-użytkownika)
4. [Komendy CLI](#komendy-cli)
5. [Obsługa podstawowa](#obsługa-podstawowa)
6. [Konfiguracja systemu](#konfiguracja-systemu)
7. [Tryby pracy](#tryby-pracy)
8. [Wyświetlacz LCD](#wyświetlacz-lcd)
9. [Pomiar rezystancji styków](#pomiar-rezystancji-styków)
10. [Reset systemu](#reset-systemu)
11. [Bezpieczeństwo](#bezpieczeństwo)
12. [Rozwiązywanie problemów](#rozwiązywanie-problemów)
13. [Dane techniczne](#dane-techniczne)

---

## Wprowadzenie

**Relay Endurance Tester** to system do długoterminowego testowania przekaźników sterujących silnikiem indukcyjnym. Urządzenie wykonuje setki tysięcy cykli przełączeń przekaźników oraz umożliwia okresowe pomiary rezystancji styków roboczych.

### Główne cechy

- Test trwałości przekaźników do **500 000+ cykli**
- Sterowanie **10 kanałami** (20 przekaźników)
- Automatyczne wznowienie testu po zaniku zasilania
- Punkty pomiarowe co określoną liczbę cykli
- Interfejs CLI przez UART
- Wyświetlacz LCD 16x2

---

## Informacje ogólne

### Sterownik

- **Arduino Mega 2560**
- Firmware w wersji określonej komendą `STATUS` (pole `FW`)

### Parametry połączenia UART

| Parametr | Wartość |
|----------|---------|
| Baudrate | 115200 |
| Data bits | 8 |
| Stop bits | 1 |
| Parity | None |
| Newline | LF |

### Kanały sterowania

System steruje 10 modułami przekaźnikowymi. Każdy moduł zawiera:

- **Przekaźnik główny** (ON/OFF)
- **Przekaźnik kierunku** (LEFT/RIGHT)

Łącznie: **20 wyjść cyfrowych**

---

## Interfejs użytkownika

### Terminal UART

Komunikacja z systemem odbywa się przez port szeregowy. Można użyć:

- Arduino Serial Monitor
- PuTTY
- CoolTerm
- inny terminal UART

### Wyświetlacz LCD

Wyświetlacz 16x2 I2C pokazuje:

- Linia 1: licznik cykli (`CYCLE:123456`)
- Linia 2: stan systemu i wzorzec kanałów

---

## Komendy CLI

### Lista wszystkich komend

| Komenda | Opis |
|---------|------|
| `PING` | Test komunikacji |
| `STATUS` | Wyświetl status systemu |
| `CONFIG` | Wyświetl konfigurację |
| `START` | Uruchom test |
| `STOP` | Zatrzymaj test |
| `CONTINUE` | Wznów test |
| `RESET` | Wyzeruj liczniki |
| `FACTORY_RESET` | Przywróć ustawienia fabryczne |
| `TEST` | Włącz tryb pomiarowy |
| `RELEASE` | Wyłącz tryb pomiarowy |
| `SET_DELAY <ms>` | Ustaw opóźnienie między krokami |
| `SET_TARGET <cycles>` | Ustaw docelową liczbę cykli |
| `SET_INTERVAL <cycles>` | Ustaw interwał pomiarowy |
| `HELP` | Wyświetl listę komend |

---

### PING

Sprawdzenie komunikacji z urządzeniem.

**Komenda:**
```
PING
```

**Odpowiedź:**
```
OK
```

---

### STATUS

Wyświetla aktualny stan systemu.

**Komenda:**
```
STATUS
```

**Odpowiedź:**
```
STATE=RUNNING;CYCLES=123456;DIR=RIGHT;TARGET=100000;POWER_FAILS=2;RUNTIME_H=48;FW=1.0
```

| Pole | Znaczenie |
|------|-----------|
| `STATE` | Aktualny stan maszyny stanów |
| `CYCLES` | Liczba wykonanych cykli |
| `DIR` | Aktualny kierunek (LEFT/RIGHT) |
| `TARGET` | Docelowa liczba cykli |
| `POWER_FAILS` | Licznik restartów zasilania |
| `RUNTIME_H` | Czas pracy testu w godzinach |
| `FW` | Wersja firmware |

---

### CONFIG

Wyświetla aktualną konfigurację.

**Komenda:**
```
CONFIG
```

**Odpowiedź:**
```
CONFIGURATION:
STEP_DELAY=1000
TARGET_CYCLES=100000
MEASURE_INTERVAL=20000
SAVE_INTERVAL=100
FW=1.0
```

| Parametr | Znaczenie |
|----------|-----------|
| `STEP_DELAY` | Czas między kolejnymi kanałami [ms] |
| `TARGET_CYCLES` | Docelowa liczba cykli testu |
| `MEASURE_INTERVAL` | Co ile cykli punkt pomiarowy |
| `SAVE_INTERVAL` | Co ile cykli zapis do EEPROM |

---

### START

Uruchamia test od aktualnego stanu.

**Komenda:**
```
START
```

**Dostępność:** Stan `IDLE`

**Błąd:** `ERR:INVALID_STATE` jeśli komenda wykonana w niedozwolonym stanie

---

### STOP

Zatrzymuje test i wyłącza wszystkie przekaźniki.

**Komenda:**
```
STOP
```

**Dostępność:** Stany testowe (`RUNNING`, `STEP_ON`, `STEP_OFF`, `WAIT_DEAD_TIME`, `CHANGE_DIRECTION`)

**Błąd:** `ERR:INVALID_STATE` jeśli komenda wykonana w niedozwolonym stanie

---

### CONTINUE

Wznawia zatrzymany test.

**Komenda:**
```
CONTINUE
```

**Dostępność:** Stany `IDLE`, `WAIT_MEASUREMENT`

**Błąd:** `ERR:INVALID_STATE` jeśli komenda wykonana w niedozwolonym stanie

---

### TEST

Włącza tryb pomiarowy – wszystkie przekaźniki główne ON, kierunkowe OFF.

**Komenda:**
```
TEST
```

**Dostępność:** Stany `IDLE`, `WAIT_MEASUREMENT`

**Błąd:** `ERR:INVALID_STATE` jeśli komenda wykonana w niedozwolonym stanie

---

### RELEASE

Wyłącza tryb pomiarowy i przechodzi do stanu `IDLE`.

**Komenda:**
```
RELEASE
```

**Dostępność:** Stan `TEST_MODE`

**Błąd:** `ERR:INVALID_STATE` jeśli komenda wykonana w niedozwolonym stanie

---

### SET_DELAY

Ustawia opóźnienie między kolejnymi krokami sekwencji.

**Komenda:**
```
SET_DELAY 1000
```

**Parametr:** Czas w milisekundach (domyślnie 1000 ms)

---

### SET_TARGET

Ustawia docelową liczbę cykli testu.

**Komenda:**
```
SET_TARGET 500000
```

**Parametr:** Liczba cykli (domyślnie 100000)

---

### SET_INTERVAL

Ustawia interwał punktów pomiarowych.

**Komenda:**
```
SET_INTERVAL 20000
```

**Parametr:** Liczba cykli między pomiarami (domyślnie 20000)

---

### RESET

Zeruje liczniki, ale **zachowuje** konfigurację.

**Komenda:**
```
RESET
```

**Odpowiedź:**
```
SYSTEM RESET
```

**Zerowane wartości:**
- `sequence` (numer sekwencji zapisu)
- `cycle_counter`
- `runtime_seconds`
- `power_fail_counter`
- `direction` (reset do LEFT)

---

### FACTORY_RESET

Przywraca ustawienia fabryczne – zeruje liczniki **i** konfigurację.

**Komenda:**
```
FACTORY_RESET
```

**Odpowiedź:**
```
FACTORY RESET OK
```

**Przywracane wartości domyślne:**
- `STEP_DELAY = 1000 ms`
- `TARGET_CYCLES = 100000`
- `MEASURE_INTERVAL = 20000`
- `SAVE_INTERVAL = 100`

---

## Obsługa podstawowa

### Uruchomienie testu

1. Podłącz urządzenie do komputera przez USB
2. Otwórz terminal UART (baudrate: 115200)
3. Sprawdź komunikację:
   ```
   PING
   ```
4. Sprawdź status:
   ```
   STATUS
   ```
5. W razie potrzeby skonfiguruj parametry:
   ```
   SET_TARGET 200000
   SET_INTERVAL 20000
   SET_DELAY 1000
   ```
6. Uruchom test:
   ```
   START
   ```

### Zatrzymanie testu

Aby zatrzymać test:
```
STOP
```

Aby wznowić:
```
CONTINUE
```

### Monitorowanie postępu

Sprawdzaj status okresowo:
```
STATUS
```

Lub obserwuj wyświetlacz LCD.

---

## Konfiguracja systemu

### Parametry konfiguracyjne

| Parametr | Domyślna wartość | Opis |
|----------|------------------|------|
| `STEP_DELAY` | 1000 ms | Czas załączenia każdego kanału |
| `TARGET_CYCLES` | 100000 | Docelowa liczba cykli |
| `MEASURE_INTERVAL` | 20000 | Interwał punktów pomiarowych |
| `SAVE_INTERVAL` | 100 | Częstotliwość zapisu EEPROM |

### Przykładowa konfiguracja dla długiego testu

```
SET_TARGET 500000
SET_INTERVAL 20000
SET_DELAY 1000
```

### Przykładowa konfiguracja dla szybkiego testu

```
SET_TARGET 1000
SET_INTERVAL 200
SET_DELAY 500
```

---

## Tryby pracy

### Sekwencja wave

W trybie `RUNNING` system wykonuje sekwencję **wave** – w danym momencie załączony jest tylko jeden kanał:

```
Kanał 1 ON
↓ (STEP_DELAY)
Kanał 1 OFF / Kanał 2 ON
↓ (STEP_DELAY)
Kanał 2 OFF / Kanał 3 ON
...
↓ (STEP_DELAY)
Kanał 9 OFF / Kanał 10 ON
↓ (STEP_DELAY)
Kanał 10 OFF
↓ (DEAD_TIME = 100 ms)
Zmiana kierunku
↓
Kolejny cykl
```

### Zmiana kierunku

Po każdym cyklu system zmienia kierunek obrotów:

```
LEFT → RIGHT → LEFT → RIGHT → ...
```

### Punkty pomiarowe

Co `MEASURE_INTERVAL` cykli system zatrzymuje się w stanie `WAIT_MEASUREMENT`.

Aby wznowić test:
```
CONTINUE
```

Aby wykonać pomiar rezystancji:
```
TEST
```

---

## Wyświetlacz LCD

### Format wyświetlania

```
Linia 1: CYCLE:123456
Linia 2: XXXX YYYYYYYYYY
```

Gdzie:
- `XXXX` – 4-znakowy skrót stanu
- `YYYYYYYYYY` – wzorzec aktywnych kanałów (1=ON, 0=OFF)

### Przykłady

```
CYCLE:123456
ON   1000000000     ← kanał 1 załączony
```

```
CYCLE:123456
ON   0100000000     ← kanał 2 załączony
```

```
CYCLE:20000
MEAS 0000000000     ← punkt pomiarowy
```

### Skróty stanów

| Stan | Skrót |
|------|-------|
| STATE_INIT | INIT |
| STATE_IDLE | IDLE |
| STATE_RUNNING | RUN |
| STATE_STEP_ON | ON |
| STATE_STEP_OFF | OFF |
| STATE_WAIT_DEAD_TIME | DEAD |
| STATE_CHANGE_DIRECTION | DIR |
| STATE_WAIT_MEASUREMENT | MEAS |
| STATE_TEST_MODE | TEST |
| STATE_FINISHED | END |
| STATE_ERROR | ERR |

---

## Pomiar rezystancji styków

### Tryb TEST

Tryb TEST umożliwia pomiar rezystancji styków roboczych przekaźników.

**Włączenie trybu:**
```
TEST
```

**W trybie TEST:**
- Wszystkie przekaźniki główne → **ON**
- Wszystkie przekaźniki kierunku → **OFF**

**Wyłączenie trybu:**
```
RELEASE
```

### Procedura pomiaru

1. Zatrzymaj test lub poczekaj na punkt pomiarowy
2. Włącz tryb TEST:
   ```
   TEST
   ```
3. Wykonaj pomiar rezystancji styków multimetrem
4. Zapisz wyniki
5. Wyłącz tryb TEST:
   ```
   RELEASE
   ```
6. Wznów test:
   ```
   CONTINUE
   ```

---

## Reset systemu

### RESET vs FACTORY_RESET

| Cecha | RESET | FACTORY_RESET |
|-------|-------|---------------|
| Zeruje licznik cykli | ✓ | ✓ |
| Zeruje czas pracy | ✓ | ✓ |
| Zeruje licznik restartów | ✓ | ✓ |
| Resetuje kierunek | ✓ | ✓ |
| Przywraca domyślną konfigurację | ✗ | ✓ |

### Kiedy użyć RESET

- Przed rozpoczęciem nowego testu
- Gdy chcesz zachować aktualną konfigurację

### Kiedy użyć FACTORY_RESET

- Gdy zmienisz wartości domyślne w firmware
- Gdy chcesz przywrócić urządzenie do ustawień fabrycznych
- Przy problemach z konfiguracją

---

## Bezpieczeństwo

### Interlock bezpieczeństwa

Przekaźnik kierunku **nie może zostać przełączony**, gdy przekaźnik główny jest załączony.

### Watchdog

System posiada watchdog sprzętowy z timeoutem **2 sekundy**. Jeśli firmware zawiesi się, mikrokontroler zostanie automatycznie zrestartowany.

### Emergency Stop

System posiada fizyczny **Emergency Stop**, który odcina zasilanie przekaźników.

### Ochrona EEPROM

- Mechanizm wear leveling (ring buffer, 271 slotów)
- Weryfikacja CRC przy odczycie
- Ponad **4600 pełnych testów** przed zużyciem pamięci EEPROM

---

## Rozwiązywanie problemów

### Brak odpowiedzi na PING

**Sprawdź:**
- Baudrate terminala (powinno być 115200)
- Wybrany port COM
- Połączenie kabla USB
- Czy Arduino jest zasilane

### Przekaźniki włączają się odwrotnie

Niektóre moduły przekaźników są typu **LOW trigger**. Wymaga to odwrócenia logiki sterowania w firmware.

### LCD nie działa

**Sprawdź:**
- Adres I2C wyświetlacza (0x27 lub 0x3F)
- Podłączenie linii SDA/SCL
- Zasilanie wyświetlacza

### Test nie wznawia się po restarcie

**Sprawdź:**
- Czy EEPROM zawiera prawidłowe dane (`STATUS`)
- Czy nie osiągnięto punktu pomiarowego (`STATE=WAIT_MEASUREMENT`)
- Czy nie osiągnięto celu (`STATE=FINISHED`)

### Stan ERROR

System przeszedł do stanu błędu krytycznego.

**Rozwiązanie:**
1. Wszystkie przekaźniki zostaną wyłączone automatycznie
2. Użyj komendy `RESET` lub `FACTORY_RESET`
3. Lub wykonaj power cycle (wyłącz i włącz zasilanie)

---

## Dane techniczne

### Czasy

| Parametr | Wartość domyślna |
|----------|------------------|
| STEP_DELAY | 1000 ms |
| DEAD_TIME | 100 ms |
| Timeout watchdog | 2 s |
| Interwał zapisu runtime | 5 min |

### Szacowany czas testu

Przy `STEP_DELAY = 1000 ms`:

| Cykle | Czas |
|-------|------|
| 100 000 | ~13 dni |
| 200 000 | ~26 dni |
| 500 000 | ~64 dni |

### EEPROM

| Parametr | Wartość |
|----------|---------|
| Slotów STATE | 271 |
| Zapisów przed zużyciem | 27 100 000 |
| Gwarancja producenta | 100 000 zapisów/komórkę |

### Zasoby pamięci

| Zasób | Wykorzystanie |
|-------|---------------|
| CONFIG | Adresy 0–31 |
| STATE ring buffer | Adresy 32–4095 |

---

## Historia wersji dokumentu

| Wersja | Data | Opis |
|--------|------|------|
| 1.0 | 2026-04-01 | Pierwsza wersja instrukcji |
