#ifndef DENONAVR_H
#define DENONAVR_H


#include "DenonVolume.h"

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <HardwareSerial.h>
#include <ESPmDNS.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <HardwareSerial.h>
#include <ESPmDNS.h>
#else
#error Platform not supported
#endif

typedef std::function<void(void*, AsyncClient*)> ConnHandler;
typedef std::function<void(void*, AsyncClient*)> DisconnHandler;
typedef std::function<void(const char *response, size_t len)> ResponseHandler;
typedef std::function<void(const char *errorMessage)> ErrorHandler;

typedef enum conType{RS232, TELNET};

class DENON_AVR {
  public:
    DENON_AVR(){}
    void _volume_feedback(const char *data,size_t len);

    AsyncClient* AVClient;
    HardwareSerial *_serialPort;

    DenonVolume Volume;
    
    bool begin(IPAddress _ip);
    bool begin();
    bool begin(HardwareSerial *serialPort);

    void onConnect(ConnHandler callbackFunc);
    void onDisconnect(DisconnHandler callbackFunc);
    void onDenonResponse(ResponseHandler callbackFunc);
    void onError(ErrorHandler callbackFunc);

    ConnHandler _connCallback = NULL;
    DisconnHandler _disconnCallback = NULL;
    ResponseHandler _denon_response_cb = NULL;
    ErrorHandler _conErr_cb = NULL;

    bool set(const char* _command, const char *_value);
    bool set(const char* _command, float i);
    String get(const char* _command);

    bool write(const char* buf, size_t i){
      if(AVClient->canSend() && AVClient->connected()){
        AVClient->write(buf,i);
        return true;
      }
      return false;
    }      
    void attachCb();

    int stoi(String i) {
      return int(i.toInt());
    }
  private:
    IPAddress avr_ip;
    conType _conType;    
};

#endif
