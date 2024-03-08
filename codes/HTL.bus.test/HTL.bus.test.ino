#include <I2C.h>
#include <Wire.h>

void setup() 
{
  Serial.begin(9600);
  delay(1000);Serial.println();
  Serial.print("MOSI: ");Serial.println(MOSI);
  Serial.print("MISO: ");Serial.println(MISO); 
  Serial.print("SCK: ");Serial.println(SCK); 
  Serial.print("SS: ");Serial.println(SS);
  Serial.print("SDA: ");Serial.println(SDA);
  Serial.print("SCL: ");Serial.println(SCL);
}

void loop()
{}
