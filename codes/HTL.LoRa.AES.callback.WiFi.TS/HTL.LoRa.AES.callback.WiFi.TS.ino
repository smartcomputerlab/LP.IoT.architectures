#include <SPI.h>              // include libraries
#include <LoRa.h>
#include "mbedtls/aes.h"          // accelerated AES module
#include "LoRa.TS.par.h"

#include <WiFi.h>
#include "ThingSpeak.h"
char ssid[] = "PhoneAP";             // your network SSID (name)
char pass[] = "smartcomputerlab";    // your network password (use for WPA, or use as key for WEP)
unsigned long myChannelNumber =1626377;   
const char *myWriteAPIKey="3IN09682SQX3PT4Z" ;
const char *myReadAPIKey="9JVTP8ZHVTB9G4TT" ;
WiFiClient  client;

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
  mbedtls_aes_setkey_dec( &aes, (const unsigned char*) key, strlen(key) * 8 );
    for(int i=0;i<nblocks;i++)
    {
      mbedtls_aes_crypt_ecb(&aes,..,
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
    Serial.println(LoRa...());
    xQueueReset(pqueue); // to keep only the last element 
    xQueueSend(pqueue,&rdp,portMAX_DELAY);  
    } 
}


void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA); 
  Serial.begin(9600); 
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500); Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");  
  ThingSpeak.begin(client); // connexion (TCP) du client au serveur
  delay(1000);
  Serial.println("ThingSpeak begin");
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
LoRa...(lora.par.bw);
LoRa.setCodingRate4(lora.par.cr);
pqueue = xQueueCreate(4,54);  // sizeof rdp union 
LoRa.onReceive(onReceive);  // link to ISR
LoRa.receive();             // reception activation
}

unsigned long lastmils=0;
int tswait=15000;           // wait for 15 sec

void loop()  // la boucle de l’emetteur
{
int packetLen;char key[17]="abcdefghijklmnop";
pack_t rdp, enc_rdp;
  xQueueReceive(pqueue,&enc_rdp,portMAX_DELAY); 
  Serial.println("Encrypted packet");
  for(int i=0; i<54;i++) Serial.print(enc_rdp.frame[i],HEX); Serial.println();
  decrypt(enc_rdp.frame+6,key,rdp.frame+6,3);
  Serial.println("Decrypted packet");
  for(int i=0; i<54;i++) Serial.print(rdp.frame[i],HEX); Serial.println();
  Serial.println("data received from the queue");  

  if((lastmils+tswait)<millis())
    {
      lastmils=millis();
      Serial.println(rdp.pay.sens[0]);  Serial.println(rdp.pay.sens[1]);
      Serial.printf("channel: %d, wkey:%16.16s\n",enc_rdp.pay.channel,rdp.pay.wkey);
      Serial.printf("flag[0]: %2.2X, flag[1]: %2.2X\n",enc_rdp.pay.flag[0],enc_rdp.pay.flag[1]);
      rdp.pay.flag[0]=0xC0;rdp.pay.flag[1]=0x00;
      
      Serial.println("Fields update");
      if(enc_rdp.pay.flag[0]&0x80) ThingSpeak.setField(1, rdp.pay.sens[0]);
      if(enc_rdp.pay.flag[0]&0x40) ThingSpeak.setField(2, rdp.pay.sens[1]);
      int x = ThingSpeak.writeFields(enc_rdp.pay.channel, rdp.pay.wkey);
      if(x == 200){Serial.println("Channel update successful.");}
        else { Serial.println("Problem updating channel. HTTP error code " + String(x));} 
    }    
  delay(200); 
}
