# ez80f91bsp

Assembler-Bibliothek für den **Zilog eZ80F91** und das Development Kit.
Sie stellt Board-, UART-/Konsolen-, Speicher- und Ethernet-Hilfen für native
ZDS-II-Anwendungen bereit. CAMELF91_24 bindet den Debug-Build als
`libbspd.lib` ein.

## Checkout und Build

```sh
git clone https://github.com/notwendig/ez80f91bsp.git
cd ez80f91bsp
```

`libbsp.zdsproj` in ZDS II eZ80Acclaim! 5.3.5 öffnen und **Debug → Rebuild All**
ausführen. Die Projektdatei stammt ursprünglich aus ZDS 5.3.4 und kann von
5.3.5 übernommen werden. Der Assembler-Includepfad enthält `inc/`.

| Konfiguration | Ausgabe |
| --- | --- |
| Debug | `libbspd.lib` |
| Release | `libbsp.lib` |

Die Bibliotheksdateien, Listings, Objekte und IDE-Sitzungen werden nicht
versioniert. Nach Quelländerungen muss die Bibliothek vor der Anwendung
neu gebaut werden.

## Module und Schnittstellen

| Quelle | Aufgabe |
| --- | --- |
| `src/bsp.asm` | Board-Pininitialisierung, kritische Abschnitte, Fehlerhalt |
| `src/uart.asm` | UART0-Konfiguration, Interrupts und Ringpuffer |
| `src/console.asm` | Konsolenfunktionen und UART0-Anbindung |
| `src/malloc.asm` | Heap-Hilfen |
| `src/emac.asm`, `src/phy.asm` | MAC-/PHY-Hilfen |
| `src/printf.asm` | Formatierte Ausgabe |

`src/queue.asm` und `src/task.asm` sind zusätzliche Quellen; sie stehen nicht
in der aktuellen Modulauswahl von `libbsp.zdsproj`. Ihre bloße Anwesenheit
ist kein Nachweis eines geprüften Schedulers oder aller API-Pfade.
Die Include-Dateien unter `inc/` dokumentieren die jeweiligen Register- und
Parameterkonventionen.

## UART und Anwendungseinbindung

`init_bsp` initialisiert die Board-Pins; es ersetzt **nicht** den Aufruf von
`init_uart0`. Vor der ersten Ausgabe muss die Anwendung eine gültige
`UARTCFG` anlegen, deren Adresse in `IY` übergeben und `init_uart0` aufrufen.
Die Struktur umfasst Baudteiler, Leitungseinstellung und vier Pufferschwellen.
Die Konfiguration hängt vom tatsächlich verwendeten Systemtakt ab.

Die Konsolenschnittstelle bietet unter anderem `putc`, `puts`, `getc`,
`gets`, `kbhit` und `flush`; Zeichen für `putc` stehen in `A`.
UART-Interrupts benötigen eine eingerichtete Vektortabelle, `_set_vector`
und freigegebene CPU-Interrupts. Der native UART0-Treiber verwendet RTS/CTS.

Das Anwendungsprojekt muss den F91-C-/Boardstart und die passenden
Linkerdefinitionen bereitstellen. `test.zdsproj` samt `test/` ist das
getrennte Anwendungsbeispiel; `libbsp.zdsproj` baut allein die Bibliothek.

## Prüfung und Grenzen

Der native Debug-Build von
[CAMELF91_24](https://github.com/notwendig/CAMELF91_24) prüft die UART-Konsole
zusammen mit Forth, Interruptbetrieb und Reset im
[qemuez80f91-Workspace](https://github.com/notwendig/qemuez80f91).
Das ist kein vollständiger Hardwaretest aller BSP-Module, insbesondere nicht
aller EMAC-, Heap-, Queue- oder Task-Funktionen.

Die vorhandenen Urheber- und Nutzungshinweise der Quelldateien bleiben
maßgeblich; dieses README fügt keine pauschale Lizenzfreigabe hinzu.
