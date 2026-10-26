# Proposed low-voltage bench wiring

Classic ESP32 DevKit only. No hardware was assembled or tested.

| MFRC522 pin | ESP32 |
| --- | --- |
| 3.3V | 3.3 V, never 5 V |
| GND | Common GND |
| SCK | GPIO18 |
| MISO | GPIO19 |
| MOSI | GPIO23 |
| SDA / SS | GPIO21 (SPI chip select, not I2C) |
| RST | GPIO22 |
| IRQ | Unused |

Relay signal: GPIO26 to a suitable 3.3 V-compatible active-high relay module input.
Use an appropriate external supply and driver/module; never drive a bare relay coil
from a GPIO. The draft writes LOW for locked and HIGH for the three-second unlock
request. Many relay boards are active-low and will require changed logic. Add a
hardware pull-down if appropriate for the chosen input, and measure reset/brownout
states before connecting any load. Start with an LED or dummy low-voltage load.

No mains wiring or real door lock is specified. Lock power, flyback suppression,
contact ratings, emergency release, fail-safe/fail-secure behaviour and local rules
need a separately engineered design. The software cannot guarantee physical locking.
