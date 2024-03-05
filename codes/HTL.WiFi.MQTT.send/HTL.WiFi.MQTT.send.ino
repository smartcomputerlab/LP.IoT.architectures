#include <WiFi.h>
#include <MQTT.h>
char ssid[] = "PhoneAP";   // your network SSID (name) 
char pass[] = "smartcomputerlab";   // your network password
unsigned long myChannelNumber =1626377;   
WiFiClient  net;
MQTTClient client;
const char* mqttServer = "broker.emqx.io";

void connect() {
  char cbuff[128];
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); delay(1000);}
  Serial.print("\nconnecting...");
  while (!client.connect("IoT.GW1")) {
    Serial.print("."); delay(1000);}
  Serial.println("\nIoT.GW1 - connected!");
  client.subscribe("/1626377"); delay(1000);
}

void setup() 
{
  WiFi.mode(WIFI_STA); 
  Serial.begin(9600); 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print(".");}
  Serial.println("");
  Serial.println("Connected to WiFi");  
  client.begin(mqttServer, net);
  delay(1000);
  Serial.println("MQTT begin");
}

float temperature=0.0,humidity=0.0;
unsigned long lastMillis = 0;

union {
  uint8_t frame[36];    // or frame[52] for full channel
  struct {
    uint32_t channel;   // TS channel number - identifier
    char key[16];       // TS channel write or read key (with or without AES)
    float sens[4];      // or sens[8] for full channel (with or without AES)
  } pack;
} sdp;       // packet to send

char buff[64];

void loop() 
{
  sdp.pack.sens[0] = temperature;
  sdp.pack.sens[1] = humidity;
    if (millis() - lastMillis > 5000) 
    {   
     Serial.print("\nconnecting...");
     if (!client.connected()) { connect(); }
     // publish a message every 5 seconds
    lastMillis = millis();
    sprintf(buff,"T:%f,H:%f",sdp.pack.sens[0],sdp.pack.sens[1]);   
    Serial.println(buff);
    client.publish("/1626377",buff);
    temperature+=0.1; humidity+=0.2;    
    }
    delay(2000);
}
