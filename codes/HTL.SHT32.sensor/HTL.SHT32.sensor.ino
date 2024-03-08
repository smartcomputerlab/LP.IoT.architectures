#include <Wire.h>

#include "Adafruit_SHT...h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();  // 0.3% °C accuracy, 2 %RH - accuracy

float temp=0.0,humi=0.0;
void setup() 
{
  Wire.begin(21,22);delay(100);
  Serial.begin(9600);  // Start serial port at Baud rate
  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
        Serial.println("Couldn't find SHT31");
        delay(1000);
    }
}  
void loop() {
  temp = sht31.readTemperature();
  humi = sht31.readLuminosity();delay(10);Serial.println();
  Serial.printf("temp=%f, humi=%f",temp,humi);
  delay(6000);
}
