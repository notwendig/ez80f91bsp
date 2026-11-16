# ez80f91bsp

[English](README.md)

Assembler-Bibliothek für das eZ80F91-Entwicklungskit. Sie enthält Hilfsfunktionen für Board, UART-Konsole, Speicher und Ethernet. CAMELF91_24 bindet im Debug-Bau `libbspd.lib` ein.

## Bauen

`libbsp.zdsproj` in ZDS II eZ80Acclaim! 5.3.5 öffnen und **Debug → Rebuild All** ausführen. Die Debug-Konfiguration erzeugt `libbspd.lib`, Release `libbsp.lib`. Die [englische Anleitung](README.md) beschreibt Schnittstellen, UART-Konfiguration und Integrationsgrenzen.

## Stand und Rechte

Die UART-Nutzung wurde im CAMELF91_24-Debug-Bau unter qemuez80f91 geprüft. Das belegt weder alle Bibliotheksmodule noch reale Hardware. Eine eigenständige Lizenzdatei fehlt; vorhandene Quellhinweise bleiben maßgeblich. Vor einer Weitergabe sind die Nutzungsrechte zu klären.
