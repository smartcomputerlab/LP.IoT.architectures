#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "LoRa.TS.par.h"
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
  pinMode(DI0,INPUT);  // signal interrupt                   
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  readEEPROM(&ts, &lora);
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


float d1=12.0, d2=321.54 ;

void loop()  // la boucle de l’emetteur
{
Serial.println("New Packet") ;
  LoRa.beginPacket();                   // start packet
  sdp.pay.sens[0]=d1;
  sdp.pay.sens[1]=d2;
  LoRa.write(sdp.frame,54);
  LoRa.endPacket();
d1++; d2+=2;
delay(2000);
}
