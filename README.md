# ez80f91bsp

Assembly library for the **Zilog eZ80F91** development kit. It provides board,
UART/console, memory and Ethernet helpers for native ZDS II applications.
CAMELF91_24 links its Debug output as `libbspd.lib`.

## Checkout and build

```sh
git clone https://github.com/notwendig/ez80f91bsp.git
cd ez80f91bsp
```

Open `libbsp.zdsproj` in ZDS II eZ80Acclaim! 5.3.5 and select
**Debug → Rebuild All**. The project originated in 5.3.4 and can be used by
5.3.5. The assembler include path contains `inc/`.

| Configuration | Output |
| --- | --- |
| Debug | `libbspd.lib` |
| Release | `libbsp.lib` |

Library outputs, listings, objects and IDE sessions are not tracked. Rebuild
the library before the application after source changes.

## Modules and interfaces

| Source | Purpose |
| --- | --- |
| `src/bsp.asm` | Board pins, critical sections and failure halt |
| `src/uart.asm` | UART0 setup, interrupts and ring buffers |
| `src/console.asm` | Console functions and UART0 connection |
| `src/malloc.asm` | Heap helpers |
| `src/emac.asm`, `src/phy.asm` | MAC/PHY helpers |
| `src/printf.asm` | Formatted output |

Additional `src/queue.asm` and `src/task.asm` are not selected by the current
library project. Their presence does not establish a tested scheduler or complete
API coverage. Includes in `inc/` document register/parameter conventions.

## UART and application integration

`init_bsp` initializes board pins; it does **not** replace `init_uart0`.
Before output, create a valid `UARTCFG`, pass its address in IY and call
`init_uart0`. The structure contains baud divisor, line configuration and four
buffer thresholds, matched to the actual system clock.

Console functions include `putc`, `puts`, `getc`, `gets`, `kbhit` and `flush`;
`putc` receives its character in A. UART interrupts require vector initialization,
`_set_vector` and enabled CPU interrupts. The native UART0 driver uses RTS/CTS.

The application supplies F91 C/board startup and suitable linker definitions.
`test.zdsproj` and `test/` form a separate example; `libbsp.zdsproj` builds only
the library.

## Evidence and limits

The native [CAMELF91_24](https://github.com/notwendig/CAMELF91_24) Debug build
exercises the UART console with Forth, interrupts and reset in
[qemuez80f91](https://github.com/notwendig/qemuez80f91). This does not establish
all BSP modules on hardware, particularly EMAC, heap, queue or task APIs.
Existing copyright and usage notices remain authoritative; this README adds no
blanket license grant.
