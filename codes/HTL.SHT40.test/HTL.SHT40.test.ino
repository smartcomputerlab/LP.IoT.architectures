#include <Wire.h>

#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Asdafruit_SHT4x();

float temp=0.0,humi=0.0;
sensors_event_t shumi, stemp;

void setup() 
{
  Wire.begin(..);delay(10);
  Serial.begin(9600);  // Start serial port at Baud rate
  if (!sht4.begin()) {   
        Serial.println("Couldn't find SHT40");
        delay(1000);
    }
  Serial.println("");
}   

void loop()
{
  uint32_t timestamp = millis();
  sht4.getEvent(&shumi, &stemp);
  timestamp = millis() - timestamp;
    temp = stemp.temperature;
  humi = shumi.relative_humidity;delay(10);Serial.println();
  Serial.printf("temp=%f, humi=%f\n",temp,humi);
  Serial.print("Read duration (ms): ");
  Serial.println(timestamp);
 delay(3000);
}
