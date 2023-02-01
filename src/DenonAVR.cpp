#include "DenonAVR.h"



bool DENON_AVR::begin(){
  _conType = conType::TELNET;
  String toMatch = "Denon";
  if (!MDNS.begin("ESP32")) {
    return false;
  }
  int n = MDNS.queryService("http", "tcp");
  if (n == 0) {
       MDNS.end();
      return false;
  } else {
    for (int i = 0; i < n; ++i) {
      if(MDNS.hostname(i).indexOf("Denon")>=0) avr_ip = MDNS.IP(i);
    }
  }
  MDNS.end();
  AVClient = new AsyncClient;
  attachCb();
  if(AVClient->connect(avr_ip,23)){
    
    return true;
  }
  // MDNS.end();
  return false;
}

bool DENON_AVR::begin(IPAddress _ip){
  _conType = conType::TELNET;
  AVClient = new AsyncClient;
  attachCb();
  if(AVClient->connect(_ip,23)){
    avr_ip = _ip;
    return true;
  }
  return false;
}

bool DENON_AVR::begin(HardwareSerial *serialPort){
  _serialPort = serialPort;
  _conType = conType::RS232;
  attachCb();  
  _serialPort->begin(9600);
  return true;
}

void DENON_AVR::attachCb(){
  if(_conType == conType::TELNET){
    if(_connCallback != NULL) AVClient->onConnect(_connCallback, AVClient);
    if(_denon_response_cb != NULL) AVClient->onData([=](void*, AsyncClient*, void *data, size_t len){
      _denon_response_cb((const char *)data, len);
    });
    if(_disconnCallback != NULL) AVClient->onDisconnect(_disconnCallback, AVClient);
    if(_conErr_cb != NULL) AVClient->onError([=](void*, AsyncClient*, int8_t error){
      _conErr_cb(AVClient->errorToString(error));
    });
  }

  else if(_conType == conType::RS232){
    if(_denon_response_cb != NULL) _serialPort->onReceive([=](void) {
      while(_serialPort->available()){
        size_t l = _serialPort->available();
        uint8_t b[l];
        l = _serialPort->read(b, l);
        _denon_response_cb((const char *)b, l);
      }
    });
    if(_conErr_cb != NULL) _serialPort->onReceiveError([=](hardwareSerial_error_t error) {
      int err = error;
      if(err == 0) _conErr_cb("No Error");
      else if(err == 1) _conErr_cb("Connection broken");
      else if(err == 2) _conErr_cb("Buffer Full");
      else if(err == 3) _conErr_cb("FiFo Overflow");
      else if(err == 4) _conErr_cb("Frame error");
      else if(err == 5) _conErr_cb("Parity Error");
    });
  }
}


bool DENON_AVR::set(const char* _command, const char *_value){
  char buf[20];
  sprintf(buf, "%s%s\r",_command,_value);

  if(_conType == conType::TELNET){  
    if(AVClient->canSend() && AVClient->connected()){
      AVClient->write(buf);
      return true;
    }
    return false;
  }
  else{ //if(_conType == conType::RS232){
    _serialPort->write(buf);
    return true;
  }
  return false;
}

bool DENON_AVR::set(const char* _command, int i){
  char buf[20];

  sprintf(buf, "%s%d\r",_command,i);

  if(_conType == conType::TELNET){
    if(AVClient->canSend() && AVClient->connected()){
      AVClient->write(buf);
      return true;
    }
    return false;
  }
  else{ //if(_conType == conType::RS232){
    _serialPort->write(buf);
    return true;
  }
  return false;
}
  

//Callbacks for Telnet only
void DENON_AVR::onConnect(ConnHandler callbackFunc) {
  _connCallback = callbackFunc;
}
void DENON_AVR::onDisconnect(DisconnHandler callbackFunc) {
  _disconnCallback = callbackFunc;
}
//generalized callback
void DENON_AVR::onDenonResponse(ResponseHandler callbackFunc){
  _denon_response_cb = callbackFunc;
}
void DENON_AVR::onError(ErrorHandler callbackFunc){
  _conErr_cb = callbackFunc;
}
