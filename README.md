# Denon_AVR_control
A fully asynchron Denon® AVR control library over Telnet or UART for ESP32 in Arduino IDE

# Dependecies
- ESP32 Core 2.0    - https://github.com/espressif/arduino-esp32
- WiFi.h            - build in in ESP32 Core - https://github.com/espressif/arduino-esp32
- HardwareSerial.h  - build in in ESP32 Core - https://github.com/espressif/arduino-esp32
- ESPmDNS.h         - https://github.com/espressif/arduino-esp32
- AsyncTCP.h        - https://github.com/me-no-dev/AsyncTCP

# Usage

This library makes use of the AsyncTCP library and or the HardwareSerial Library and is thereby fully async. 
There is no need to call any repeated function in the Loop.
Received messages are called in callback functions. If you only want to send Commands to an AVR no callback function has to be used.

There is a Table in the "commands.h" file that has the basic Commands.

The begin functions are:
- bool begin(IPAddress _ip); -> comunication over Telnet with knon IP Adress
- bool begin(); -> communication over Telnet with unknown IP Adess (DNS lookup)
- bool begin(HardwareSerial *serialPort); -> communication over UART


The callback functions are:
- void onConnect(ConnHandler callbackFunc); // Telnet only
- void onDisconnect(DisconnHandler callbackFunc); // Telnet only
- void onDenonResponse(ResponseHandler callbackFunc); // Telnet & UART
- void onError(ErrorHandler callbackFunc); // Telnet & UART


Functions to send a command to the AVR:
- bool set(const char* _command, const char *_value);
- bool set(const char* _command, int i);
- String get(const char* _command);
