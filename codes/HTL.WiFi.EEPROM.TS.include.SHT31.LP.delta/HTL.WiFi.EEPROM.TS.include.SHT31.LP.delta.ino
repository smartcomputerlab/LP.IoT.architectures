#include <WiFi.h>
#include "WiFiTSpar.h"  // ap_serv_t, ts_par_t, readEEPROM(&servpar_r,&tspar_r)
#include "ThingSpeak.h"
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31(); // 0.3% °C accuracy, 2 %RH - accuracy
#define uS_TO_S_FACTOR 1000000ULL     // Conversion factor for micro seconds to seconds 
#define TIME_TO_SLEEP  15             // Time ESP32 will go to sleep (in seconds)
RTC_DATA_ATTR int bootCount = 0;

RTC_DATA_ATTR float stemp=0.0,shumi=0.0; // last saved sensor data
int delta=10; // 10 => 10*0.0001 => O.1%

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

int delta_test(float curr, float saved, int delta)
{
if(curr> (saved*(1.0+delta*0.0001)) || curr<(saved*(1.0-delta*0.0001))) return(1);
else return(0);
}

float temp=0.0,humi=0.0;
int tosend=0;

void setup() 
{
  char kbuff[17];
  Serial.begin(9600); delay(100);
  Wire.begin(21,22);delay(10);
  if (!sht31.begin(0x44)) { // Set to 0x45 for alternate i2c addr
  Serial.println("Couldn't find SHT31"); delay(1000);}
  Serial.println("");
  temp = sht31.readTemperature();
  humi = sht31.readHumidity();delay(10);Serial.println();
  Serial.printf("temp=%f, humi=%f\n",temp,humi);
  sdp.pay.sens[0] = temp;sdp.pay.sens[1] = humi;
  tosend=delta_test(temp,stemp,delta); 
  Serial.printf("data to send =1 or not=0 : %d\n",tosend);
  if(tosend)
    { 
    stemp=temp; shumi=humi; // new send values
    sdp.pay.flag[0]=0xC0;sdp.pay.flag[1]=0x00;
    readEEPROM(&ts_serv,&ts_par);  // this operation loads the pre-defined parameters to structures
    WiFi.mode(WIFI_STA);   delay(10);
    Serial.println(ts_serv.par.ssid);  Serial.println(ts_serv.par.pass);
    WiFi.begin(ts_serv.par.ssid, ts_serv.par.pass);delay(10);
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
    }   
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Sleep for every " + String(TIME_TO_SLEEP) +" sec"); 
  Serial.println("Going to sleep now");
  Serial.flush(); 
  WiFi.disconnect();delay(40);
  esp_deep_sleep_start();
}

void loop()
{
  Serial.println("This will never be printed");
}
