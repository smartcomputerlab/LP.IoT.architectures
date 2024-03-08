#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "mbedtls/aes.h"          // accelerated AES module
#include "LoRa.TS.par.h"
#include <Wire.h>
#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31 = Adafruit_SHT31();
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;
#define uS_TO_S_FACTOR 1000000 SLEEP
uint32_t time_sec=TIME_TO_WAIT;

#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    17   // GPIO27 -- SX127x's MOSI
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
pack_t sdp, enc_sdp; 

void encrypt(char *plainText, char *key, unsigned char *outputBuffer, int nblocks){

  mbedtls_aes_context aes;
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*)key, strlen(key) * 8 );
    for(int i=0;i<nblocks;i++)
    {
    mbedtls_aes_crypt_ecb( &aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)(plainText+i*16), outputBuffer+i*16);
   }
  mbedtls_aes_free( &aes );
}
typedef union
{
  uint8_t frame[22];
  struct 
  {
    uint32_t  channel;      // thingspeak channel number - identifier
    char      flag[2];      // validation-information flags
    float     data[4];      // ACK data or parameters: cycle time,delta,kpacket, etc.
  } pay;
} ack_t;
ack_t ack, enc_ack;     // for clear and encrypted ACK packet

void decrypt(unsigned char *chipherText,char *key,unsigned char *outputBuffer, int nblocks)
{
  mbedtls_aes_context aes;
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_dec( &aes, (const unsigned char*) key, strlen(key) * 8 );
    for(int i=0;i<nblocks;i++)
    {
      mbedtls_aes_crypt_ecb(&aes,MBEDTLS_AES_DECRYPT,
                       (const unsigned char*)(chipherText+i*16), outputBuffer+i*16);
    }                   
  mbedtls_aes_free(&aes );
}

QueueHandle_t ackqueue;   // packet queue

void ..Receive(int packetSize) 
{ 
int i=0;
ack_t enc_ack;

  if(packetSize==0) return;   // if there's no packet, return 
  i=0; 
  if (packetSize==22) 
    { 
    while (LoRa.available()) { 
      enc_ack.frame[i]=LoRa.read();i++; 
      } 
    Serial.println(LoRa.packetRssi());
    xQueueReset(ackqueue); // to keep only the last element 
    xQueueSend(ackqueue,&enc_ack,portMAX_DELAY);  
    } 
}

float temp,humi;

void readSHT31()
{
  Wire.begin(21,22);delay(100);
  if(!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
        Serial.println("Couldn't find SHT31");
        delay(1000);
    }
  temp = sht31.readTemperature();
  humi = sht31.readHumidity();delay(100);Serial.println();
  Wire.end();
}


void setup() {
  ack_t ack,enc_ack;
  BaseType_t qret;   // from FreeRTOS
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
  LoRa.setCodingRate44(lora.par.cr);
  ackqueue = xQueueCreate(4,22);  // sizeof rdp union
  LoRa.onReceive(onReceive);  // link to ISR
  char key[17]="abcdefghijklmnop";
  Serial.println("New Packet") ;
  LoRa.beginPacket();   
  // start packet
  readSHT31();
  // data to be encrypted
  strncpy(sdp.pay.wkey,"3IN09682SQX3PT4Z",16);
  sdp.pay.sens[0]=temp;
  sdp.pay.sens[1]=humi;
  Serial.printf("temp=%f, humi=%f\n",temp,humi);
  Serial.println(key);
  Serial.println("\nClear packet");
  for(int i=6; i<54;i++) Serial.print(sdp.frame[i],HEX);Serial.println();
  encrypt((char *)sdp.frame+6, key, enc_sdp.frame+6,3); 
  Serial.println("Encrypted packet");
  for(int i=6; i<54;i++) Serial.print(enc_sdp.frame[i],HEX); Serial.println();
  // non ecrypted data: channel number and flags
  enc_sdp.pay.channel=1626377;
  enc_sdp.pay.flag[0]=0xC0;
  LoRa.write(enc_sdp.frame,54);
  LoRa.endPacket();
  ..
  qret=xQueueReceive(ackqueue,&enc_ack,1000); // portMAX_DELAY); - in milliseconds
  if(qret==pdTRUE)
    {
    Serial.println("data received from the queue");
    Serial.println("Encrypted ACK packet");
    if(enc_sdp.pay.channel==enc_ack.pay.channel)
      {
      for(int i=0; i<22;i++) Serial.print(enc_ack.frame[i],HEX); Serial.println();
      decrypt(enc_ack.frame+6,key,ack.frame+6,1);
      Serial.println("Decrypted ACK packet");
      for(int i=0; i<22;i++) Serial.print(ack.frame[i],HEX); Serial.println();
      Serial.printf("channel: %d, flag[0]: %2.2X\n",enc_ack.pay.channel,enc_ack.pay.flag[0]);
      Serial.printf("cycle: %d, delta: %d\n",(int)ack.pay.data[0],(int)ack.pay.data[1]);
      }
    else Serial.println("not my ACK channel"); 
    }
  esp_sleep_enable_timer_wakeup((uint32_t)time_sec * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(time_sec) + " Seconds");
  esp_deep_sleep_start();
}

void loop()  // la boucle de l’emetteur
{}
