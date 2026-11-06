# desk-signal

A learning project in C++ for the ESP32 controller (ESP-IDF).

The firmware reads four buttons and serves their state at `GET /state` on port 80:

```json
{"forward":false,"back":false,"left":false,"right":false,"fresh":false}
```

`fresh` is true while the button reading is under 500 ms old. The buttons are pulled up, and a press shorts the pin to ground. The LED is lit while any button is held.

| | GPIO |
| --- | --- |
| forward | 16 |
| back | 17 |
| left | 18 |
| right | 19 |
| LED | 2 |

The network credentials stay out of the commit. Copy the example and fill in `wifi_ssid` and `wifi_password`:

```bash
cp esp32/main/wifi_secret.h.example esp32/main/wifi_secret.h
```

Build and flash from the `esp32` directory. The build target is `esp32`:

```bash
cd esp32
idf.py set-target esp32
idf.py build flash monitor
```

The address to request is printed in the log after the board gets an IP.

Run the tests from the repository root. Success is the line `Test passed` and exit code 0:

```bash
esp32/tests/test.sh
```
