/*
This is an example on how to use the Telnet communication

*/


#include "commands.h"
#include "DenonAVR.h"

IPAddress DENON_IP(192,168,2,2);  // If IP of AVR is known
DENON_AVR X4400H;                 // Instance of class

//Callback Functions
//-------------------------------------------------------------------------------------------------------------------
//Callback for Telnet connection when established
void denon_connected(void* arg, AsyncClient* client) {
	Serial.printf("client has been connected at %s:%d", client->remoteIP().toString().c_str(), client->remotePort());
}
//Callback for Telnet connection when connection lost
void denon_disconnected(void* arg, AsyncClient* client) {
	Serial.print("client disconnected");
}
//Callback for Telnet or Serial when error occured
void conError(const char *errorMessage){
  Serial.print(errorMessage);
}
//Callback for Telnet or Serial when there is a Message from connected AVR
void denonResponded(const char *response, size_t len){
  Serial.write(response, len);
  Serial.write('\r');
}

//Standard setup function
//-------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);  // only do this if connection via Telnet or Serial connection to AVR is on Serial1 or Serial2
  
  WiFi.disconnect(true, true);
  delay(200);
  WiFi.mode(WIFI_STA);
  WiFi.begin("ssid","password");
  if (WiFi.waitForConnectResult() != WL_CONNECTED) return;
  WiFi.setAutoReconnect(true);

  //setup callback functions
  X4400H.onConnect(denon_connected);
  X4400H.onDisconnect(denon_disconnected);
  X4400H.onError(conError);
  X4400H.onDenonResponse(denonResponded);

  // Try to connect to AVR @ supplied IPAdress first, if not found
  // use mDNS Discovery to find the IP of a "Denon" on local network
  if(!X4400H.begin(DENON_IP)) X4400H.begin(); 

  X4400H.set(MAIN_ZONE,ON);	//main Zone switches on
  X4400H.Volume++;		//Volume goes up by 0.5
  X4400H.Volume+2;		//Volume goes up by 2
}

void loop() {
  while(Serial.available()){
    size_t l = Serial.available();
    uint8_t b[l];
    l = Serial.read(b, l);
    X4400H.write((char *)b, l);
  }
}
