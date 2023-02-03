# Denon_AVR_control
A fully asynchron Denon AVR control library over Telnet or UART for ESP32 in Arduino IDE

# Dependecies
- ESP32 Core 2.0    - https://github.com/espressif/arduino-esp32
- WiFi.h            - build in in ESP32 Core - https://github.com/espressif/arduino-esp32
- HardwareSerial.h  - build in in ESP32 Core - https://github.com/espressif/arduino-esp32
- ESPmDNS.h         - https://github.com/espressif/arduino-esp32
- AsyncTCP.h        - https://github.com/me-no-dev/AsyncTCP
- DenonVolume.h     - included in src folder

# Usage

This library makes use of the AsyncTCP library and or the HardwareSerial Library and is thereby fully async. 
There is no need to call any repeated function in the Loop.
Received messages are called in callback functions. If you only want to send Commands to an AVR no callback function has to be used.

There is a Table in the "commands.h" file that has the basic Commands.

##Initializing the class is without any constructors.
Example:

DENON_AVR X3700H;



##The begin functions (usually called in setup()) are:
- bool begin(IPAddress _ip); -> comunication over Telnet with knon IP Adress
- bool begin(); -> communication over Telnet with unknown IP Adess (DNS lookup)
- bool begin(HardwareSerial *serialPort); -> communication over UART

Example for use of Telnet with known IP:

X3700H.begin(avrIP);




##The callback functions are:
- void onConnect(ConnHandler callbackFunc); // Telnet only
- void onDisconnect(DisconnHandler callbackFunc); // Telnet only
- void onDenonResponse(ResponseHandler callbackFunc); // Telnet & UART
- void onError(ErrorHandler callbackFunc); // Telnet & UART

Example for Message callback:
X3700H.onDenonResponse(denonResponseCallbackFunction);




##Functions to send a command to the AVR:
- bool set(const char* _command, const char *_value);
- bool set(const char* _command, int i);
- String get(const char* _command);

Example to send a command for Main zone Power on:
X3700H.set(MAIN_ZONE, ON);
Example to send command for increasing Volume:
X3700H.set(MAIN_VOLUME, UP);

As the Volume control is kind of strange, I wrote a DenonVolume Library to help ease of changing Volume:
To change the Volume now all you have to do is:
- X3700H.Volume++; //Volume goes up by 0.5
- X3700H.Volume--; //Volume goes down by 0.5
- X3700H.Volume+1; //increase Volume by 1
- X3700H.Volume = 10.5 // set Volume to 10.5
it is supposed to be very intuitive and again relyes on Callbacks only, so nothing else has to be done and the commands are send to the AVR



To see a full example have a look at the example.ino file

#LICENCE
This is published under GNU Lesser Public Lincence
