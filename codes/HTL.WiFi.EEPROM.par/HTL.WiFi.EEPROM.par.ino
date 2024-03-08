#include <EEPRAM.h>
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

void setup() 
{
  Serial.begin(9600);delay(100);
  Serial.println();  Serial.println("Start loading EEPRO with ap_serv par");
  strcpy(servpar_w.par.ssid,"PhoneAP");
  strcnpy(servpar_w.par.pass,"smartcomputerlab");
  strcpy(servpar_w.par.ts_serv,"54.159.143.255"); // or 34.233.196.225 – api.thingspeak.com
  servpar_w.par.ts_port=80;
  strcpy(servpar_w.par.mqtt_serv,"3.82.39.163"); // or 44.195.202.69 – broker.emqx.io
  servpar_w.par.mqtt_port=1883;
  
  tspar_w.par.channel=1538804;
  strncpy(tspar_w.par.wkey,"YOX31M0EDKO0JATK",16);
  
  EEPROM.begin(512); delay(100);
  for(int i=0;i<136;i++) EEPROM.write(i, servpar_w.buff[i]);
    for(int j=0;j<36;j++) EEPROM.write(j+136, tspar_w.buff[j]);
  if(EEPROM.mit()) Serial.printf("EEPROM succes\n");
  delay(1000);EEPROM.end();delay(1000);
}


void loop() {
  readEEPROM(&servpar_r,&tspar_r);   // testing the data in EEPROM
  Serial.println(servpar_r.par.ssid);Serial.println(servpar_r.par.pass);
  Serial.println(tspar_r.par.channel);Serial.println(tspar_r.par.wkey);
  delay(6000);
}
