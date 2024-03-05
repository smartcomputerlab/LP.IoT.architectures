#include <esp_now.h>
#include <WiFi.h>
#include <MQTT.h>
WiFiClient  net;
MQTTClient client;
char ssid[] = "PhoneAP";   // your network SSID (name) 
char pass[] = "smartcomputerlab";   // your network password
unsigned long myChannelNumber =1626377; 
const char* mqttServer = "broker.emqx.io";

void messageReceived(String &topic, String &payload) 
{
  char *ptr=0;
  char pay[128]; float temp=23.6;
  payload.toCharArray(pay,128);
  Serial.println("incoming: " + topic + " - " + payload);
  //ptr=strstr(pay,"T:");temp=atof(ptr+2);  // here we need correct string to convert
  //Serial.println(ptr); Serial.println(temp);
}

void connect() {
  char cbuff[128];
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) { Serial.print(".");  delay(1000);}
  Serial.print("\nconnecting...");
  while (!client.connect("IoT.GW2")) { Serial.print("."); delay(1000);}
  Serial.println("\nIoT.GW1 - connected!");
  client.subscribe("/1626377");
  delay(1000);
}

void setup() {
  Serial.begin(9600);Serial.println();
  WiFi.mode(WIFI_STA); 
  Serial.begin(9600); 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print(".");}
  delay(200); Serial.println(); 
  Serial.println("WiFi connected");
  client.begin(mqttServer, net);
  client.onMessage(messageReceived);
  connect();
}

void loop() 
{
  char buff[64];
  client.loop();
  delay(4000);}

  
