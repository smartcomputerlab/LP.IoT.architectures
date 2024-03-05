#include <WiFi.h>
#include "ThingSpeak.h"
char ssid[] = "PhoneAP";             // your network SSID (name)
char pass[] = "smartcomputerlab";    // your network password (use for WPA, or use as key for WEP)
unsigned long myChannelNumber =1626377;   
const char *myWriteAPIKey="3IN09682SQX3PT4Z" ;
WiFiClient  client;
RTC_DATA_ATTR int bootCount = 0;
int sig=0;

void TimerWakeUp_setExternalInput(int level){
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,level);   //(GPIO_NUM_33,1); 
  Serial.printf("Wakeup ESP32 when IO33 at level %d\n",level);
}

int level = 1;  // to test the PIR sensor

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial.println();
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print(".");}
  Serial.println("");
  Serial.println("Connected to WiFi");  
  ThingSpeak.begin(client); // connexion (TCP) du client au serveur
  Serial.println("Start PIR test interrupt !");
  bootCount++;Serial.print(bootCount);delay(100);
  Serial.println("Fields update");
  ThingSpeak.setField(3,(float) bootCount);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){Serial.println("Channel update successful.");}
    else { Serial.println("Problem updating channel. HTTP error code " + String(x));} 
  Serial.println(bootCount);
  WiFi.disconnect();
  delay(3000);  // wait to evoid 3 sec remanency value on PIR sensor
  bootCount++;
  TimerWakeUp_setExternalInput(level);
  esp_deep_sleep_start();
}

void loop()
{}
