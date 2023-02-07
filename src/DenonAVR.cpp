#include "DenonAVR.h"

bool DENON_AVR::begin() {
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
      if (MDNS.hostname(i).indexOf("Denon") >= 0) avr_ip = MDNS.IP(i);
    }
  }
  MDNS.end();
  AVClient = new AsyncClient;
  attachCb();
  if (AVClient->connect(avr_ip, 23)) {

    return true;
  }
  return false;
}

bool DENON_AVR::begin(IPAddress _ip) {
  _conType = conType::TELNET;
  AVClient = new AsyncClient;
  attachCb();
  if (AVClient->connect(_ip, 23)) {
    avr_ip = _ip;
    return true;
  }
  return false;
}

bool DENON_AVR::begin(HardwareSerial *serialPort) {
  _serialPort = serialPort;
  _conType = conType::RS232;
  attachCb();
  _serialPort->begin(9600);
  return true;
}

void DENON_AVR::_volume_feedback(const char *data, size_t len) {
  String input(data);
  if (input.indexOf("MV") >= 0 && len < 7) {
    String sub(input.substring(input.indexOf("MV") + 2, len));
    float j = sub.toInt();
    String sub2(input.charAt(4));
    if (sub2.toInt() == 5) j /= 10;
    this->Volume.is = j;
  }
}

void DENON_AVR::attachCb() {
  if (_conType == conType::TELNET) {
    if (_connCallback != NULL) AVClient->onConnect(_connCallback, AVClient);
    if (_denon_response_cb != NULL) AVClient->onData([=](void *, AsyncClient *, void *data, size_t len) {
      _denon_response_cb((const char *)data, len);
      _volume_feedback((const char *)data, len);  //this-> entfernt !!!!!!!!!!!!!!!!!!!!
      //neu eingefügt
      for (int i = 0; i < 10; i++) {
        if (_SateCallback[i] != 0) _SateCallback[i](_StateCallback_arg, (const char *)data, len, no[i]);
      }
      //ende neu einfügt
    },
                                                     this);  //this eingefügt!!!!!!!!!!!
    if (_disconnCallback != NULL) AVClient->onDisconnect(_disconnCallback, AVClient);
    if (_conErr_cb != NULL) AVClient->onError([=](void *, AsyncClient *, int8_t error) {
      _conErr_cb(AVClient->errorToString(error));
    });
  }

  else if (_conType == conType::RS232) {
    if (_denon_response_cb != NULL) _serialPort->onReceive([=](void) {
      while (_serialPort->available()) {
        size_t l = _serialPort->available();
        uint8_t b[l];
        l = _serialPort->read(b, l);
        _denon_response_cb((const char *)b, l);
        _volume_feedback((const char *)b, l);  //this-> entfernt !!!!!!!!!!!!!!!!
        //neu eingefügt
        for (int i = 0; i < 10; i++) {
          if (_SateCallback[i] != 0) _SateCallback[i](_StateCallback_arg, (const char *)b,l, no[i]);
        }
        //ende neu einfügt
      }
    },
                                                           this);  //this eingefügt!!!!!!!!!
    if (_conErr_cb != NULL) _serialPort->onReceiveError([=](hardwareSerial_error_t error) {
      int err = error;
      if (err == 0) _conErr_cb("No Error");
      else if (err == 1) _conErr_cb("Connection broken");
      else if (err == 2) _conErr_cb("Buffer Full");
      else if (err == 3) _conErr_cb("FiFo Overflow");
      else if (err == 4) _conErr_cb("Frame error");
      else if (err == 5) _conErr_cb("Parity Error");
    });
  }

  Volume.onVolChange([=](void *, char *vol) {
    this->set("MV", vol);
  },
                     this);
}


bool DENON_AVR::set(const char *_command, const char *_value) {
  char buf[20];
  sprintf(buf, "%s%s\r", _command, _value);

  if (_conType == conType::TELNET) {
    if (AVClient->canSend() && AVClient->connected()) {
      AVClient->write(buf);
      return true;
    }
    return false;
  } else {  //if(_conType == conType::RS232){
    _serialPort->write(buf);
    return true;
  }
  return false;
}

bool DENON_AVR::set(const char *_command, float is) {
  char buf1[4];
  int kommazahl = (int)is;
  is -= (float)kommazahl;
  is *= (float)10;
  int a = kommazahl;
  int b = is;
  if (is != 0) sprintf(buf1, "%02d%d", a, b);  //sprintf(buf, "%02d%d", (int)kommazahl, (int)is)
  else sprintf(buf1, "%02d", kommazahl);
  char buf[23];
  sprintf(buf, "%s%s\r", _command, buf1);
  if (_conType == conType::TELNET) {
    if (AVClient->canSend() && AVClient->connected()) {
      AVClient->write(buf);
      return true;
    }
    return false;
  } else {  //if(_conType == conType::RS232){
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
void DENON_AVR::onDenonResponse(ResponseHandler callbackFunc) {
  _denon_response_cb = callbackFunc;
}
void DENON_AVR::onError(ErrorHandler callbackFunc) {
  _conErr_cb = callbackFunc;
}

//neu eingefügt
void DENON_AVR::onStateChange(StateHandler callbackFunc, void *arg, int nn) {
  _SateCallback[nn] = callbackFunc;
  _StateCallback_arg = arg;
  no[nn] = nn;
}

void DenonProperties::_feedback(const char *data, size_t len) {
  String in(data);
  String input(in.substring(0, len));
  String search(_Command);
  if (input.indexOf(search) == 0 && len < 30) {
    String sub(in.substring(search.length(), len));
    const char* k = sub.c_str();
    strncpy(is,k,sizeof(is));
    if(_state_update_cb != 0) _state_update_cb((const char *)is, strlen(is));
    if(_state_update_int_cb != 0) {
      int i = sub.toInt();
      _state_update_int_cb(i);
    }
  }
}

void DenonProperties::addCommand(const char *Command) {
  size_t s = sizeof(Command);
  _Command = new char[20];
  strcpy(_Command,Command);
  strcpy(is, "N/A");
  obj->onStateChange([=](void *,const char *state,size_t len, int nn) -> void {  //war [&]!!!!!!!!!
    _feedback(state,len);
  },
                     this, objNo);
}

// DenonProperties::DenonProperties(DENON_AVR *i, int j) {
//   obj = i;
//   obj->no[j] = j;
//   objNo = j;
// }

int DenonProperties::_objNo = 0;
DENON_AVR* DenonProperties::obj = NULL;

DenonProperties::DenonProperties(DENON_AVR* i){
  obj = i;
}

DenonProperties::DenonProperties(const char * Command, int j){
  obj->no[j] = j;
  objNo = j;
  addCommand(Command);
  _objNo++;
}

DenonProperties::DenonProperties(DENON_AVR* i, const char * Command, int j){
  obj = i;
  obj->no[j] = j;
  objNo = j;
  addCommand(Command);
  _objNo++;
}

void DenonProperties::set(const char *i) {
  obj->set(_Command, i);
  strncpy(is, i,sizeof(is));
  // is = (char *)i;
}

char *DenonProperties::get() {
  return is;
}
int DenonProperties::get(int i){
  String j(is);
  if(_state_update_int_cb != 0) return j.toInt();
  return -1;  
}

void DenonProperties::onStateUpdate(ResponseHandler callbackFunc){
  _state_update_cb = callbackFunc;
}

void DenonProperties::onStateUpdate(ResponseHandlerInt callbackFunc){
  _state_update_int_cb = callbackFunc;
}
//ende neu eingefügt
