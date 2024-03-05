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

//#define freq    868E6   
//#define sf 7
//#define sb 125E3       
//#define cr 8 

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
pack_t rdp; 

QueueHandle_t pqueue;   // packet queue

void onReceive(int packetSize) 
{ 
int i=0;
pack_t rdp; // packet to receive

  if(packetSize==0) return;   // if there's no packet, return 
  i=0; 
  if (packetSize==54) 
    { 
    while (LoRa.available()) { 
      rdp.frame[i]=LoRa.read();i++; 
      } 
    Serial.println(LoRa.packetRssi());
    xQueueReset(pqueue); // to keep only the last element 
    xQueueSend(pqueue,&rdp,portMAX_DELAY);  
    } 
}


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
pqueue = xQueueCreate(4,54);  // sizeof rdp union 
LoRa.onReceive(onReceive);  // link to ISR
LoRa.receive();             // reception activation
}

void loop()  // la boucle de l’emetteur
{
int packetLen;
pack_t rdp;
  xQueueReceive(pqueue,&rdp,portMAX_DELAY); 
  Serial.println("data received from the queue");   
  Serial.println(rdp.pay.sens[0]);  Serial.println(rdp.pay.sens[1]);
}
