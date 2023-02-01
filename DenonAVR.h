#ifndef DENONAVR_H
#define DENONAVR_H


#include <WiFi.h>
#include <HardwareSerial.h>
#include <ESPmDNS.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#else
#error Platform not supported
#endif


// IPADdress AVR_IP(192.168.1.2)
// AVR X4400H(CONNECTION_TYPE::TELNET, IP_ADRESS::KNOWN, ...
//            AVR_IP,  );
// X4400H.set(Power, ON);
// X4400H.get(Power);
// X4400H.set(Volume, UP)


typedef std::function<void(void*, AsyncClient*)> ConnHandler;
typedef std::function<void(void*, AsyncClient*)> DisconnHandler;
typedef std::function<void(const char *response, size_t len)> ResponseHandler;
typedef std::function<void(const char *errorMessage)> ErrorHandler;

typedef enum conType{RS232, TELNET};

class DENON_AVR {
  public:
    AsyncClient* AVClient;
    HardwareSerial *_serialPort;

    // DENON_AVR(WiFiClient *av_cli);
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
    bool set(const char* _command, int i);
    String get(const char* _command);

    bool write(const char* buf, size_t i){
      if(AVClient->canSend() && AVClient->connected()){
        AVClient->write(buf,i);
        return true;
      }
    }      

  private:
    IPAddress avr_ip;
    conType _conType;

    void attachCb();
    
};



#endif