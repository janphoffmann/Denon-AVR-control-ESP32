#ifndef DENONAVR_H
#define DENONAVR_H

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <HardwareSerial.h>
#include <ESPmDNS.h>
#include "DenonVolume.h"
#else
#error Platform not supported
#endif

class DENON_AVR;
class DenonProperties;


typedef std::function<void(void*, AsyncClient*)> ConnHandler;
typedef std::function<void(void*, AsyncClient*)> DisconnHandler;
typedef std::function<void(const char* response, size_t len)> ResponseHandler;
typedef std::function<void(const char* errorMessage)> ErrorHandler;
//neu eingefügt
typedef std::function<void(void*, const char* state, size_t len, int nn)> StateHandler;
typedef std::function<void(int i)> ResponseHandlerInt;

typedef enum IntOrChar { CHARACTER,
                       INTEGER };
//ende neu eingefügt
typedef enum conType { RS232,
                       TELNET };


class DenonProperties {
  
public:

  DenonProperties(DENON_AVR* i);
  DenonProperties(DENON_AVR* i, const char* Command, int j = _objNo);
  DenonProperties(const char* Command, int j = _objNo);

  static DENON_AVR* obj;
  char* _Command;
  void _feedback(const char* data, size_t len);
  void addCommand(const char* Command);
  void set(const char* i);
  char* get();
  int get(int i);
  char is[30];
  int objNo;

  void onStateUpdate(ResponseHandler callbackFunc);
  void onStateUpdate(ResponseHandlerInt callbackFunc);
  ResponseHandler _state_update_cb = NULL;
  ResponseHandlerInt _state_update_int_cb = NULL;
  static int _objNo;
};

class DENON_AVR : private DenonProperties{
public:
  DENON_AVR() : DenonProperties(this){}
  void _volume_feedback(const char* data, size_t len);

  AsyncClient* AVClient;
  HardwareSerial* _serialPort;

  DenonVolume Volume;

  bool begin(IPAddress _ip);
  bool begin();
  bool begin(HardwareSerial* serialPort);

  void onConnect(ConnHandler callbackFunc);
  void onDisconnect(DisconnHandler callbackFunc);
  void onDenonResponse(ResponseHandler callbackFunc);
  void onError(ErrorHandler callbackFunc);

  ConnHandler _connCallback = NULL;
  DisconnHandler _disconnCallback = NULL;
  ResponseHandler _denon_response_cb = NULL;
  ErrorHandler _conErr_cb = NULL;

  bool set(const char* _command, const char* _value);
  bool set(const char* _command, float i);
  String get(const char* _command);

  bool write(const char* buf, size_t i) {
    if (AVClient->canSend() && AVClient->connected()) {
      AVClient->write(buf, i);
      return true;
    }
    return false;
  }
  void attachCb();

  int stoi(String i) {
    return int(i.toInt());
  }

  void onStateChange(StateHandler callbackFunc, void* arg = 0, int nn = 0);
  StateHandler _SateCallback[10] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
  void* _StateCallback_arg = NULL;
  int no[10];

private:
  IPAddress avr_ip;
  conType _conType;
};

#endif
