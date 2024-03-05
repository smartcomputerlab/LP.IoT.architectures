#include <EEPROM.h>
typedef union      // WiFi credentials and ThingSpeak server IP and port
{
  uint8_t buff[136];
  struct 
  {
    char     ssid[32];            // ssid to WiFi
    char     pass[32];            // password
    char     ts_serv[32];         // TS server URL
    int      ts_port;             // TS server port
    char     mqtt_serv[32];       // MQTT server URL
    int      mqtt_port;           // MQTT server URL – 1883
  } par;
} ap_serv_t;
ap_serv_t servpar_w, servpar_r;

typedef union     // Thingspeak channel number and API keys
{
  uint8_t buff[36];
  struct 
  {
    uint32_t  channel;            // thingspeak channel number
    char      wkey[16];           // thingspeak write key
    char      rkey[16];           // thingspeak read key
  } par;
} ts_par_t;
ts_par_t tspar_w, tspar_r;

void readEEPROM(ap_serv_t *apserv,ts_par_t *tspar)
{
  EEPROM.begin(512); delay(100);
  Serial.println("Start to read from EEPROM");
  for(int i=0;i<136;i++)apserv->buff[i]=EEPROM.read(i);
  for(int j=0;j<36;j++) tspar->buff[j]=EEPROM.read(j+136);
}

