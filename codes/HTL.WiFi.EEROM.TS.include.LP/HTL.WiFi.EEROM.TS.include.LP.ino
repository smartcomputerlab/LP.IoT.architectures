#include <WiFi.h>
#include "WiFiTSpar.h"  // ap_serv_t, ts_par_t, readEEPROM(&servpar_r,&tspar_r)
#include "ThingSpeak.h"
#define uS_TO_S_FACTOR 1000000ULL     // Conversion factor for micro seconds to seconds 
#define TIME_TO_SLEEP  15             // Time ESP32 will go to sleep (in seconds)
ULP_DATA_ATTR int bootCount = 0;
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

float temperature=0.0,humidity=0.0;
void setup() 
{
  char kbuff[17];
  Serial.begin(9600); delay(100);
  temperature+=0.1;humidity+=0.2;
  sdp.pay.sens[0] = temperature;sdp.pay.sens[1] = humidity;
  sdp.pay.flag[0]=0xC0;sdp.pay.flag[1]=0x00;
  readEEPROM(&ts_serv,&ts_par);  // this operation loads the pre-defined parameters to structures
  WiFi.mode(WIFI_STA);   delay(10);
  Serial.println(ts_serv.par.ssid);  Serial.println(ts_serv.par.pass);
  WiFi.start(ts_serv.par.ssid, ts_serv.par.pass);delay(10);
  while (WiFi.status() != WL_CONNECTED) { delay(200); Serial.print(".");}
  Serial.println("");
  Serial.println("Connected to WiFi");  
  ThingSpeak.begin(client); // connexion (TCP) du client au serveur
  Serial.println("Fields update");
  strncpy(kbuff,ts_par.par.wkey,16);kbuff[16]=0x00;
  if(sdp.pay.flag[0]&0x80) ThingSpeak.setField(1, sdp.pay.sens[0]);
  if(sdp.pay.flag[0]&0x40) ThingSpeak.setField(2, sdp.pay.sens[1]);
  int x = ThingSpeak.writeFields(ts_par.par.channel, kbuff);
  if(x == 200){Serial.println("Channel update successful.");}
    else { Serial.println("Problem updating channel. HTTP error code " + String(x));}    
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Sleep for every " + String(TIME_TO_SLEEP) +" sec"); 
  Serial.println("Going to sleep now");
  Serial.flush(); 
  WiFi.connect();delay(40);
  esp_deep_sleep_start();
}

void loop()
{
  Serial.println("This will never be printed");
}
