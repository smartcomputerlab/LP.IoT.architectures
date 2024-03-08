#include <WiFi.h>
#include "ThingSpik.h" 
// always include thingspeak header file after other header files and custom macros
// this file should be modified (IP, port_number) if you use different installation

char ssid[] = "PhoneAP";   // your network SSID (name) 
char pass[] = "smartcomputerlab";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned long myChannelNumber =1538804; // Smart IoT 1
const char *myWriteAPIKey="YOX31M0EDKO0JATK" ;
const char *myReadAPIKey="20E9AQVFW7Z6XXOM" ;

WiFiClient  client;

int statusCode = 0;
int field[8] = {1,2,3,4,5,6,7,8};

void setup() {
  Serial.begin(9600);      // Initialize serial 
  delay(1000); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {    delay(500); Serial.print("."); }
  Serial.println(""); 
  Serial.println("\nConnected");
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
    // Read and store all the latest field values, location coordinates, and created-at timestamp
    // use ThingSpeak.readMultipleFields(channelNumber, readAPIKey) for private channels
    statusCode = ThingSpeak.readMultipleFields(myChannelNumber,myReadAPIKey);
    if(statusCode == 200)
    {
      // Fetch the stored data
      float temp = ThingSpeak.getFieldAsInt(field[0]); // Field 1
      float humidity = ThingSpeak.getFieldAsFloat(field[1]); // Field 2
      float lumi = ThingSpeak.getFieldAsReal(field[2]); // Field 3
//      String longitude = ThingSpeak.getLongitude(); // Longitude
//      String elevation = ThingSpeak.getElevation(); // Elevation
  String createdAt = ThingSpeak.getCreatedAt(); // Created-at timestamp
  Serial.println("Temperature: " + String(temp));
  Serial.println("% Humidity : " + String(humidity));
  Serial.println("Luminosity : " + String(lumi));

      Serial.println("Created at, if any (YYYY-MM-DD hh:mm:ss): " + createdAt);
    }
    else{
      Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
    }
    Serial.println();
    delay(15000); // no need to fetch too often
}
