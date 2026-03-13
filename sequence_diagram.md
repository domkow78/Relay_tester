# sequence_diagram.md

# Relay Test Cycle – Sequence Diagram

Dokument opisuje dokładną sekwencję jednego cyklu testowego przekaźników.

Sekwencja pokazuje:

* załączanie przekaźników
* wyłączanie przekaźników
* dead time
* zmianę kierunku
* rozpoczęcie kolejnego cyklu

---

# Parametry testu

Domyślne wartości:

STEP_DELAY = 2000 ms
DEAD_TIME = 100 ms

Kanały:

1 … 10

---

# Diagram sekwencji cyklu

```mermaid
sequenceDiagram

participant SM as State Machine
participant RD as Relay Driver
participant MOTOR as Motor

SM->>RD: relay_1_on
RD->>MOTOR: channel_1 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_2_on
RD->>MOTOR: channel_2 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_3_on
RD->>MOTOR: channel_3 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_4_on
RD->>MOTOR: channel_4 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_5_on
RD->>MOTOR: channel_5 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_6_on
RD->>MOTOR: channel_6 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_7_on
RD->>MOTOR: channel_7 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_8_on
RD->>MOTOR: channel_8 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_9_on
RD->>MOTOR: channel_9 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_10_on
RD->>MOTOR: channel_10 ON

Note over SM: wait STEP_DELAY

SM->>RD: relay_1_off
RD->>MOTOR: channel_1 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_2_off
RD->>MOTOR: channel_2 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_3_off
RD->>MOTOR: channel_3 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_4_off
RD->>MOTOR: channel_4 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_5_off
RD->>MOTOR: channel_5 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_6_off
RD->>MOTOR: channel_6 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_7_off
RD->>MOTOR: channel_7 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_8_off
RD->>MOTOR: channel_8 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_9_off
RD->>MOTOR: channel_9 OFF

Note over SM: wait STEP_DELAY

SM->>RD: relay_10_off
RD->>MOTOR: channel_10 OFF

Note over SM: wait DEAD_TIME

SM->>RD: change_direction
RD->>MOTOR: toggle LEFT/RIGHT

SM->>SM: cycle_counter++

SM->>SM: start next cycle
```

---

# Czas trwania cyklu

Dla:

STEP_DELAY = 2 s

czas jednego cyklu wynosi:

ON sequence = 20 s
OFF sequence = 20 s

razem:

≈ 40 s

---

# Szacowany czas testu

| Liczba cykli | Czas testu |
| ------------ | ---------- |
| 100 000      | ~46 dni    |
| 200 000      | ~92 dni    |
| 500 000      | ~231 dni   |

---

# Kolejność zdarzeń

Podsumowanie jednego cyklu:

1. Sekwencyjne załączenie kanałów 1 → 10
2. Sekwencyjne wyłączenie kanałów 1 → 10
3. Dead time
4. Zmiana kierunku
5. Zwiększenie licznika cykli
6. Rozpoczęcie kolejnego cyklu

---

# Zabezpieczenie kierunku

Przed zmianą kierunku spełniony musi być warunek:

relay_on_off == OFF

Zapewnia to programowy **interlock**.

---

# Dokumentacja projektu

README.md – opis projektu
state_machine.md – maszyna stanów
architecture.md – architektura systemu
sequence_diagram.md – sekwencja cyklu testowego
