# Pin Mapping – Relay Endurance Tester

Dokument opisuje mapowanie pinów Arduino Mega 2560 używanych w testerze przekaźników.

---

## Przegląd

System używa **20 pinów cyfrowych** do sterowania 10 modułami przekaźnikowymi:

- **10 pinów relay_on** – sterowanie przekaźnikami głównymi (ON/OFF)
- **10 pinów relay_dir** – sterowanie przekaźnikami kierunku (LEFT/RIGHT)

---

## Tabela mapowania pinów

### Przekaźniki główne (relay_on)

| Kanał | Pin Arduino | Port AVR | Funkcja |
|-------|-------------|----------|---------|
| 0 | 19 | PD2 | relay_on[0] |
| 1 | 17 | PH0 | relay_on[1] |
| 2 | 15 | PJ0 | relay_on[2] |
| 3 | 23 | PA1 | relay_on[3] |
| 4 | 3 | PE5 | relay_on[4] |
| 5 | 5 | PE3 | relay_on[5] |
| 6 | 7 | PH4 | relay_on[6] |
| 7 | 9 | PH6 | relay_on[7] |
| 8 | 11 | PB5 | relay_on[8] |
| 9 | 30 | PC7 | relay_on[9] |

### Przekaźniki kierunku (relay_dir)

| Kanał | Pin Arduino | Port AVR | Funkcja |
|-------|-------------|----------|---------|
| 0 | 18 | PD3 | relay_dir[0] |
| 1 | 16 | PH1 | relay_dir[1] |
| 2 | 14 | PJ1 | relay_dir[2] |
| 3 | 22 | PA0 | relay_dir[3] |
| 4 | 2 | PE4 | relay_dir[4] |
| 5 | 4 | PG5 | relay_dir[5] |
| 6 | 6 | PH3 | relay_dir[6] |
| 7 | 8 | PH5 | relay_dir[7] |
| 8 | 10 | PB4 | relay_dir[8] |
| 9 | 12 | PB6 | relay_dir[9] |

---

## Piny zarezerwowane

Następujące piny **nie są używane** przez sterowanie przekaźników:

| Pin | Funkcja | Powód |
|-----|---------|-------|
| 0 | RX0 | UART – komunikacja z PC |
| 1 | TX0 | UART – komunikacja z PC |
| 13 | LED_BUILTIN | Bootloader używa tego pinu |
| 20 | SDA | I2C – wyświetlacz LCD |
| 21 | SCL | I2C – wyświetlacz LCD |

---

## Diagram portów AVR

```
Port A: PA0 (pin 22), PA1 (pin 23)
Port B: PB4 (pin 10), PB5 (pin 11), PB6 (pin 12)
Port C: PC7 (pin 30)
Port D: PD2 (pin 19), PD3 (pin 18)
Port E: PE3 (pin 5), PE4 (pin 2), PE5 (pin 3)
Port G: PG5 (pin 4)
Port H: PH0 (pin 17), PH1 (pin 16), PH3 (pin 6), PH4 (pin 7), PH5 (pin 8), PH6 (pin 9)
Port J: PJ0 (pin 15), PJ1 (pin 14)
```

---

## Inicjalizacja pinów

Piny są inicjalizowane w dwóch etapach:

### 1. earlyPinSafeInit()

Szybka inicjalizacja przez bezpośredni dostęp do rejestrów AVR.
Wykonywana jako pierwsza instrukcja w `setup()`.

Ustawia wszystkie piny jako **OUTPUT LOW** w kilka mikrosekund.

### 2. initRelays()

Standardowa inicjalizacja przez Arduino API (`pinMode`, `digitalWrite`).
Potwierdza ustawienia z `earlyPinSafeInit()`.

---

## Historia zmian

| Data | Zmiana |
|------|--------|
| 2026-04-10 | Pin 27 → 30 (relay_on[9]), pin 26 → 12 (relay_dir[9]) |
| Wcześniej | Pin 13 → 27 (unikanie konfliktu z bootloaderem) |
| Wcześniej | Pin 12 → 26 (unikanie konfliktu z bootloaderem) |
| Wcześniej | Pin 1 → 23 (unikanie konfliktu z UART TX) |
| Wcześniej | Pin 0 → 22 (unikanie konfliktu z UART RX) |

---

## Uwagi

- Bootloader Arduino Mega używa pinu 13 do sygnalizacji LED podczas programowania
- Piny 0/1 są zarezerwowane dla komunikacji UART z PC
- Piny 20/21 są używane przez magistralę I2C (LCD)
- Wszystkie piny sterujące przekaźnikami są inicjalizowane jako OUTPUT LOW przy starcie
