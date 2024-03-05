#include <WiFi.h>
#include "WiFiTSpar.h"  // ap_serv_t, ts_par_t, readEEPROM(&servpar_r,&tspar_r)
#include "ThingSpeak.h"

ap_serv_t ts_serv;
ts_par_t ts_par;

typedef union
{
  uint8_t buff[34];
  struct 
  {
    uint8_t   flag[2];            // TS channel number
    float     sens[8];           // thingspeak write key
  } pay;
} dtpack_t;
dtpack_t sdp;

WiFiClient  client;

void setup() 
{
  WiFi.mode(WIFI_STA); 
  Serial.begin(9600); 
  readEEPROM(&ts_serv,&ts_par);  // this operation loads the pre-defined parameters to structures
  Serial.println(ts_serv.par.ssid);  Serial.println(ts_serv.par.pass);
  WiFi.begin(ts_serv.par.ssid, ts_serv.par.pass);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500); Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");  
  ThingSpeak.begin(client); // connexion (TCP) du client au serveur
  delay(1000);
  Serial.println("ThingSpeak begin");
  sdp.pay.flag[0]=0xF0;
}

float temperature=0.0,humidity=0.0;
void loop() 
{
  char kbuff[17];
  strncpy(kbuff,ts_par.par.wkey,16);kbuff[16]=0x00;
  sdp.pay.sens[0] = temperature;sdp.pay.sens[1] = humidity;
  sdp.pay.flag[0]=0xC0;sdp.pay.flag[1]=0x00;
  Serial.println("Fields update");
  if(sdp.pay.flag[0]&0x80) ThingSpeak.setField(1, sdp.pay.sens[0]);
  if(sdp.pay.flag[0]&0x40) ThingSpeak.setField(2, sdp.pay.sens[1]);
  int x = ThingSpeak.writeFields(ts_par.par.channel, kbuff);
  if(x == 200){Serial.println("Channel update successful.");}
    else { Serial.println("Problem updating channel. HTTP error code " + String(x));} 
  delay(20000);  // required for free account
  temperature+=0.1;humidity+=0.2;    
}
