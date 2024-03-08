#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "mbedtls/...h"          // accelerated AES module
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
pack_t rdp; 

void decrypt(unsigned char *chipherText,char *key,unsigned char *outputBuffer, int nblocks)
{
  mbedtls_aes_context aes;
  mbedtls_aes_init( &aes );
  mbedtls_aes_.._dec( &aes, (const unsigned char*) key, strlen(key) * 8 );
    for(int i=0;i<nblocks;i++)
    {
      mbedtls_aes_crypt_ecb(&aes,MBEDTLS_AES_DECRYPT,
                       (const unsigned char*)(chipherText+i*16), outputBuffer+i*16);
    }                   
  mbedtls_aes_free(&aes );
}

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
LoRa...();             // reception activation
}

void loop()  // la boucle de l’emetteur
{
int packetLen;char key[17]="abcdefghijklmnop";
pack_t rdp, enc_rdp;
  xQueueReceive(pqueue,&enc_rdp,..); 
  Serial.println("Encrypted packet");
  for(int i=6; i<54;i++) Serial.print(enc_rdp.frame[i],HEX); Serial.println();
  decrypt(enc_rdp.frame+6,key,rdp.frame+6,3);
  Serial.println("data received from the queue");   
  Serial.println(rdp.pay.sens[0]);  Serial.println(rdp.pay.sens[1]);
}
