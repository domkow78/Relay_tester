# Relay Endurance Tester – Sterowanie

**UART:** 115200 baud, 8N1, LF

## Komendy

| Komenda | Opis |
|---------|------|
| `PING` | Test komunikacji → `OK` |
| `STATUS` | Wyświetl status systemu |
| `CONFIG` | Wyświetl konfigurację |
| `START` | Uruchom/wznów test |
| `STOP` | Zatrzymaj test |
| `TEST` | Tryb pomiarowy (wszystkie ON) |
| `RELEASE` | Wyłącz tryb pomiarowy |
| `RESET` | Zeruj liczniki (zachowaj config) |
| `FACTORY_RESET` | Przywróć ustawienia fabryczne |
| `SET_DELAY <ms>` | Ustaw opóźnienie kroku (domyślnie 1000) |
| `SET_TARGET <n>` | Ustaw docelową liczbę cykli (domyślnie 100000) |
| `SET_INTERVAL <n>` | Ustaw interwał pomiarowy (domyślnie 20000) |

## Uruchomienie testu
```
PING              ← test komunikacji
STATUS            ← sprawdź stan
START             ← uruchom test
```

## Pomiar rezystancji
```
TEST              ← włącz tryb pomiarowy (wszystkie przekaźniki ON)
RELEASE           ← wyłącz tryb pomiarowy
START             ← wznów test
```

---
*Relay Endurance Tester v1.0*
