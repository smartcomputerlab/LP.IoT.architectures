#include <WiFi.h>
#include <EEPROM.h>
#include "ThingSpeak.h"

typedef union
{
  uint8_t buff[136];
  struct 
  {
    char     ssid[32];            // ssid to WiFi
    char     pass[32];            // password
    char     ts_serv[32];         // TS server URL - if explicit
    int      ts_port;             // TS server port - if explicit
    char     mqtt_serv[32];       // MQTT server URL
    int      mqtt_port;           // MQTT server URL – 1883
  } par;
} ap_serv_t;                      // access point & IoT server URL-port
ap_serv_t ts_serv;

typedef union
{
  uint8_t buff[36];
  struct   {
    uint32_t  channel;            // TS channel number
    char      wkey[16];           // thingspeak write key
    char      rkey[16];           // thingspeak read key
  } par;
} ts_par_t;                       // ThingSpeak server channel & keys
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

void readEEPROM(ap_serv_t *apserv,ts_par_t *tspar)
{
  EEPROM.begin(512); delay(100);
  Serial.println("Start to read from EEPROM");
  for(int i=0;i<136;i++)apserv->buff[i]=EEPROM.read(i);
  for(int j=0;j<36;j++) tspar->buff[j]=EEPROM.read(j+136);
}


void setup() 
{
  WiFi.mode(WIFI_STA); 
  Serial.begin(9600); 
  readEEPROM(&ts_serv,&ts_par);
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
