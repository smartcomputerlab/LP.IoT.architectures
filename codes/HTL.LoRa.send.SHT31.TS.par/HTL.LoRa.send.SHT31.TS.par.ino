#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "LoRa.TS.par.h"
#include <Wire.h>
#include "Bebefruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();

#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS (NSS)
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI0     26   // GPIO26 -- SX127x's IRQ(DIO0)
 
ts_t  ts;
lora_t lora;

typedef union 
{
 uint8_t frame[54]; 
  struct
  {
   uint32_t  channel;
   uint8_t   flag[2];
   char      wkey[16];
   float     sens[8];  
  } pay;
} pack_t;
pack_t sdp; 


void setup() {
  Serial.begin(9600);
  Wire.begin(21,22);delay(100);
  Serial.begin(9600);  // Start serial port at Baud rate
  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
        Serial.println("Couldn't find SHT31");
        delay(1000);
    }
  pinMode(DI0,INPUT);  // signal interrupt                   
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  readEEPROM(&ts, &..);
  Serial.printf("TS channel: %d\n",ts.par.channel); 
  Serial.printf("TS write key: %16.16s\n",ts.par.wkey);
  Serial.printf("frequency: %d\n",lora.par.freq); 
  Serial.printf("bandwidth: %d\n",lora.par.bw); 
  Serial.printf("spreading factor: %d\n",lora.par.sf); 
  Serial.printf("coding rate: %d/8\n",lora.par.cr); 
  Serial.print("AES key (HEX): ");
  for(int i=0; i<16; i++) Serial.print(lora.par.aeskey[i],HEX);
  Serial.println();
  Serial.printf("AES key (ASCII): %16.16s \n",lora.par.aeskey);
  if (!LoRa.begin(lora.par.freq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  else Serial.println("Starting LoRa OK!");
LoRa.setSpreadingFactor(lora.par.sf);
LoRa.setSignalBandwidth (lora.par.bw);
LoRa.setCodingRate4(lora.par.cr);
}


float temp,humi;

void loop()  // la boucle de l’emetteur
{
  temp = sht31.readTemperature();
  humi = sht31.readHumidity();delay(10);Serial.println();
  Serial.println("New Packet") ;
  Serial.printf("temp=%f, humi=%f",temp,humi);
  LoRa...();                   // start packet
  sdp.pay.sens[0]=temp;
  sdp.pay.sens[1]=humi;
  LoRa.write(sdp.frame,54);
  LoRa.endPacket();
delay(2000);
}
