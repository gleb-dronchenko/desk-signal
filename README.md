# desk-signal

Учебный проект на C++ для контроллера ESP32 (ESP-IDF).

Прошивка читает четыре кнопки и отдаёт их состояние по `GET /state` на порту 80:

```json
{"forward":false,"back":false,"left":false,"right":false,"fresh":false}
```

`fresh` истинен, пока чтение кнопок не старше 500 мс. Кнопки с подтяжкой к питанию, нажатие замыкает на землю. Светодиод горит, пока зажата хотя бы одна.

| | GPIO |
| --- | --- |
| forward | 16 |
| back | 17 |
| left | 18 |
| right | 19 |
| светодиод | 2 |

Сеть не коммитится. Скопируйте пример и впишите `wifi_ssid` и `wifi_password`:

```bash
cp esp32/main/wifi_secret.h.example esp32/main/wifi_secret.h
```

Сборка и прошивка из каталога `esp32`. Цель сборки — `esp32`:

```bash
cd esp32
idf.py set-target esp32
idf.py build flash monitor
```

Адрес для запроса пишется в лог после получения IP.

Тесты с корня репозитория. Успех — строка `Test passed` и код выхода 0:

```bash
esp32/tests/test.sh
```
