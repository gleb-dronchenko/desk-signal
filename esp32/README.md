# esp32

This directory is the ESP-IDF project `desk-signal`. The build target is `esp32`.

| Button | GPIO |
| --- | --- |
| forward | `GPIO16` |
| back | `GPIO17` |
| left | `GPIO18` |
| right | `GPIO19` |

The LED is on `GPIO2`. The HTTP port is 80.

Copy the Wi-Fi secret next to the example. The copy stays out of the commit:

```bash
cp esp32/main/wifi_secret.h.example esp32/main/wifi_secret.h
```

Fill in `wifi_ssid` and `wifi_password` in the copy.

Run the host tests from the repository root:

```bash
esp32/host/test.sh
```

Success is the line `Test passed` and exit code 0.