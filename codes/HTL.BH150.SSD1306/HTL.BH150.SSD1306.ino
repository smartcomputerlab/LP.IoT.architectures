#include <Wire.h>  
#include <BH1750.h>  
BH1750 lightMeter;            
#include <U8x8lib.h>   
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15,4,16); // clock, data, reset
  

void dispData(float df1,float df2, float df3) 
{ 
  char dbuf1[32],dbuf2[32],dbuf3[32]; 
  u8x8.clear(); 
  Serial.println("title"); 
  u8x8.drawString(0,1,"title"); // 0 – colonne (max 15), 1 – ligne (max 7)
  sprintf(dbuf1,"Lux=  %2.2f",df1);u8x8.drawString(0,2,dbuf1); 
  sprintf(dbuf2,"Data2=%2.2f",df2);u8x8.drawString(0,3,dbuf2); 
  sprintf(dbuf3,"Data3=%2.2f",df3);u8x8.drawString(0,4,dbuf3); 
}

void setup() {
  Serial.begin(9600);
  u8x8.begin();  // initialize OLED
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin(21,22);
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  Serial.println(F("BH1750 One-Time Test"));
}

void loop()
{
  Serial.println("in the loop");
  while (!lightMeter.measurementReady(true)) {
    yield();
  }
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  dispData(lux,0.0,0.0);
  lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
  delay(4000);
}
