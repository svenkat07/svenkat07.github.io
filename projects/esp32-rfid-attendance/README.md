# ESP32 RFID Access Control and Attendance Prototype

**Software-only reconstruction, October 2026. AI-assisted implementation.**
This is newly reconstructed work based on the project concept in my CV. Original
source, images and measurements are unavailable. It does not reproduce original
experimental results. C++ host logic and Python mock processing have been tested;
the ESP32 firmware and actual Firebase writes have not been validated.

## Scope

The ESP32 draft reads an MFRC522 over SPI, compares demo UIDs, pulses an active-high
relay output for three seconds, and emits JSON decisions over USB serial.
The host simulator exercises the same C++ controller. A Python adapter converts
grant events into a local mock attendance database, or optionally sends a captured
file to Firebase Realtime Database over HTTPS when explicitly configured.

**Architecture difference from the original concept:** this reconstruction uses a
host-side Firebase adapter. Direct ESP32-to-Firebase Wi-Fi upload, a serial-capture
utility, login/token refresh and a live dashboard are not implemented.

## Run the software demo

Requires g++ with C++17 and Python 3; no third-party host dependencies.

```sh
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -Iinclude tests/test_access.cpp -o build/test_access
./build/test_access
python3 -m unittest discover -s tests -p 'test_*.py' -v
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -Iinclude sim/main.cpp -o build/simulator
./build/simulator > examples/simulated_events.jsonl
python3 tools/attendance.py examples/simulated_events.jsonl --session demo-session-01
```

The last command writes `examples/mock_firebase.json` without making network
requests. Every example uses invented identities and `simulated: true`.

| Logical time | Input | Expected decision |
| --- | --- | --- |
| 0 ms | demo card 01 | granted |
| 500 ms | repeated card 01 | duplicate |
| 1000 ms | card 02 while relay open | busy |
| 3000 ms | unknown card | denied |
| 4000 ms | demo card 02 | granted |
| 4500 ms | malformed UID | invalid |
| 7500 ms | demo card 01 again | granted |

The adapter records three grants. A grant is an access decision, **not proof that
a person entered**, and does not establish daily attendance. There is no door-position
sensor or people counter. Duplicate suppression covers consecutive identical scans
within two seconds; it is not general replay protection. Busy scans do not extend
the unlock pulse. The shared controller handles unsigned-millisecond timer wrap.

## Firebase integration draft

By default the adapter is local-only. To test against your own development database,
set `FIREBASE_DATABASE_URL` and a valid short-lived `FIREBASE_ID_TOKEN` in your shell,
configure restrictive authenticated-user rules, and explicitly add `--send`:

```sh
python3 tools/attendance.py examples/simulated_events.jsonl --session demo-session-01 --send
```

Do not commit tokens, use a production attendance database, or enable public database
writes. The adapter uses HTTPS certificate verification from Python's standard
library. It writes under `/attendance_demo/<event-key>` using PUT. Keys are deterministic
for a session, event uptime and subject, so retrying the same session overwrites the
same records rather than duplicating them. Use a new unique session after each boot;
otherwise events can collide. Server timestamps describe receipt time, not original
scan time. A local mock retains the Firebase timestamp placeholder, not a real timestamp.
A retry overwrites server receipt time. The adapter stops on its first failed request;
there is no persistent retry service, automatic token refresh or per-event delivery log.
No cloud request was performed while preparing this repository.

## ESP32 integration draft

Target: classic ESP32 DevKit/WROOM, PlatformIO Arduino framework. Build with `pio run`
after installing PlatformIO. The pinned baseline is provided but was not compiled here.
See [docs/wiring.md](docs/wiring.md). Capture JSON serial lines at 115200 baud to a file
before using the host adapter. No real people or access credentials are included.

UID comparison is a demonstration only. UIDs can be cloned; this is not secure access
control and is unsuitable for a real occupied-building door. Production work would
need authenticated credentials, tamper handling, secure provisioning, privacy controls,
an appropriate lock controller and independent emergency-egress compliance.
Reset/brownout relay behaviour must be verified on actual hardware.

## Validation

Executed: native C++ tests for grant/deny, malformed input, duplicate/busy handling,
exact relay expiry, case-sensitive UID validation and timer wrap; Python tests for
filtering, idempotent keys and session validation; deterministic 7-event replay.

Not executed: embedded compilation, RF/card reads, relay actuation, power/reset
behaviour, USB capture, live Firebase authentication/rules/writes or physical attendance.

## Accurate portfolio description

“Reconstructed an RFID access-control prototype with ESP32/MFRC522 firmware draft,
C++ decision and relay-timing logic, and a Firebase REST adapter. Tested synthetic
access events and local mock attendance processing; hardware and cloud validation pending.”

## Primary references

- MFRC522 SPI library and security limitations: https://github.com/miguelbalboa/rfid
- Firebase REST writes: https://firebase.google.com/docs/database/rest/save-data
- Firebase authentication: https://firebase.google.com/docs/database/rest/auth

Review and understand the AI-assisted code before presenting it in an interview.
