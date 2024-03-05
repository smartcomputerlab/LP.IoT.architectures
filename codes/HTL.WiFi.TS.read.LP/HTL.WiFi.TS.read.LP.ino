#include <WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;

char ssid[] = "PhoneAP";   // your network SSID (name) 
char pass[] = "smartcomputerlab";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned long myChannelNumber =1538804; // Smart IoT 1
const char *myWriteAPIKey="YOX31M0EDKO0JATK" ;
const char *myReadAPIKey="20E9AQVFW7Z6XXOM" ;

WiFiClient  client;

int statusCode = 0;
int field[8] = {1,2,3,4,5,6,7,8};

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("External signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("External signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(9600);      // Initialize serial 
  delay(10); 
    ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();
  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, pass);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500); Serial.print(".");}
  Serial.println(""); 
  Serial.println("\nConnected");
  ThingSpeak.begin(client);  // Initialize ThingSpeak
    // Read and store all the latest field values, ..
    // use ThingSpeak.readMultipleFields(channelNumber, readAPIKey) for private channels
    statusCode = ThingSpeak.readMultipleFields(myChannelNumber,myReadAPIKey);
    if(statusCode == 200)
    {
      float temp = ThingSpeak.getFieldAsInt(field[0]); // Field 1
      float humidity = ThingSpeak.getFieldAsFloat(field[1]); // Field 2
      float lumi = ThingSpeak.getFieldAsInt(field[2]); // Field 3
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
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +" Seconds");
    Serial.println("Going to sleep now");
    Serial.flush();  WiFi.disconnect();delay(100);
    esp_deep_sleep_start();
    Serial.println("This will never be printed");    
}


void loop(){
  //This is not going to be called
}
