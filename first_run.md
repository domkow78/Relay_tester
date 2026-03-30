# Relay Tester – Pierwsze uruchomienie

Instrukcja pierwszego uruchomienia testera przekaźników opartego na **Arduino Mega 2560**.
Celem jest bezpieczne sprawdzenie działania firmware zanim zostanie podłączony silnik i docelowy układ przekaźników.

---

# 1. Przygotowanie

## Wymagania

* Arduino Mega 2560
* komputer z Arduino IDE
* kabel USB
* terminal UART (Arduino Serial Monitor / Putty / CoolTerm)
* opcjonalnie: diody LED zamiast przekaźników

## Parametry portu szeregowego

```
baudrate: 115200
newline: LF
```

---

# 2. Wgranie firmware

1. Otwórz projekt w Arduino IDE
2. Wybierz płytkę:

```
Arduino Mega 2560
```

3. Wybierz odpowiedni port COM
4. Kliknij **Upload**

Po wgraniu firmware uruchomi się automatycznie.

---

# 3. Pierwszy test komunikacji

Otwórz terminal UART i wpisz:

```
PING
```

Odpowiedź powinna być:

```
OK
```

Jeżeli tak jest, komunikacja działa poprawnie.

---

# 4. Sprawdzenie statusu systemu

Wpisz:

```
STATUS
```

Przykładowa odpowiedź:

```
STATE=RUNNING;CYCLES=0;DIR=LEFT;TARGET=100000;POWER_FAILS=1;RUNTIME_H=0;FW=1.0
```

Opis pól:

| Pole        | Znaczenie                    |
| ----------- | ---------------------------- |
| STATE       | aktualny stan maszyny stanów |
| CYCLES      | liczba wykonanych cykli      |
| DIR         | aktualny kierunek            |
| TARGET      | docelowa liczba cykli        |
| POWER_FAILS | licznik restartów zasilania  |
| RUNTIME_H   | czas pracy testu w godzinach |
| FW          | wersja firmware              |

---

# 5. Sprawdzenie konfiguracji

Wpisz:

```
CONFIG
```

Odpowiedź powinna wyglądać np.:

```
CONFIGURATION:
STEP_DELAY=1000
TARGET_CYCLES=100000
MEASURE_INTERVAL=20000
SAVE_INTERVAL=10
FW=1.0
```

Opis parametrów:

| parametr         | znaczenie                                |
| ---------------- | ---------------------------------------- |
| STEP_DELAY       | czas pomiędzy kolejnymi przekaźnikami    |
| TARGET_CYCLES    | docelowa liczba cykli testu              |
| MEASURE_INTERVAL | co ile cykli następuje przerwa na pomiar |
| SAVE_INTERVAL    | co ile cykli zapis EEPROM                |

---

# 6. Test trybu TEST (bezpieczny test sprzętu)

Tryb TEST pozwala sprawdzić przekaźniki bez wykonywania cyklu.

Wpisz:

```
TEST
```

System powinien:

* wyłączyć wszystkie przekaźniki kierunku
* włączyć wszystkie przekaźniki główne

Następnie wpisz:

```
RELEASE
```

Powinno nastąpić wyłączenie wszystkich przekaźników.

---

# 7. Test sekwencji przekaźników

Aby szybciej zobaczyć działanie systemu ustaw krótszy czas:

```
SET_DELAY 500
```

Uruchom test:

```
START
```

Sekwencja działania powinna być następująca:

```
relay_1 ON
relay_2 ON
relay_3 ON
...
relay_10 ON

relay_1 OFF
relay_2 OFF
...
relay_10 OFF
```

Po zakończeniu cyklu zmienia się kierunek.

---

# 8. Test zapisu EEPROM

1. Pozwól wykonać kilka cykli
2. Odłącz zasilanie Arduino
3. Podłącz ponownie

Po uruchomieniu wpisz:

```
STATUS
```

Licznik cykli powinien zostać zachowany.

---

# 9. Sprawdzenie LCD

Wyświetlacz LCD powinien pokazywać np.:

```
CYC:123
ON   1000000000
```

Linia 2 składa się z 4-znakowego skrótu stanu oraz wzorca aktywnych kanałów.

Poza sekwencją załączania linia 2 wyświetla:

```
CYC:20000
MEAS 0000000000
```

---

# 10. Krótki test funkcjonalny

Przed uruchomieniem długiego testu ustaw krótszy test:

```
SET_TARGET 200
SET_INTERVAL 50
SET_DELAY 500
```

Następnie:

```
START
```

System wykona 200 cykli i zakończy test.

---

# 11. Podłączenie docelowego układu

Dopiero po przejściu wszystkich powyższych testów można podłączyć:

* moduły przekaźników
* układ zmiany kierunku
* silnik testowy

---

# 12. Najczęstsze problemy

## Brak odpowiedzi na PING

Sprawdź:

* baudrate
* port COM
* kabel USB

---

## Przekaźniki włączają się odwrotnie

Niektóre moduły przekaźników są:

```
LOW trigger
```

Wtedy trzeba odwrócić logikę sterowania.

---

## LCD nie działa

Sprawdź:

* adres I2C (0x27 / 0x3F)
* podłączenie SDA/SCL

---

# 13. Zalecenie

Pierwszy test długoterminowy wykonaj z parametrami:

```
SET_TARGET 1000
SET_INTERVAL 200
SET_DELAY 1000
```

Dopiero po poprawnym przejściu testu zwiększ parametry do docelowych wartości.
