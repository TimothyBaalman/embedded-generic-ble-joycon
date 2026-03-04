# ESP32 Arduino

; platformio.ini \
[env:esp32dev] \
platform = espressif32@^6.5.0 \
board = esp32dev \
framework = arduino \
monitor_speed = 115200

; Use NimBLE-Arduino (lighter than the classic BLE library)\
lib_deps = 
   h2zero/NimBLE-Arduino@^2.1.0

build_flags =
   -std=gnu++17