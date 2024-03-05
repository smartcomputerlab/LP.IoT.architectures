#include <SPI.h>              // include libraries
#include <LoRa.h>
#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS (NSS)
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI0     26   // GPIO26 -- SX127x's IRQ(DIO0)
#define freq    868E6   
#define sf 7
#define sb 125E3       
#define cr 8 

float d1,d2;
int i=0;

int rssi;

typedef union 
{
  uint8_t frame[16]; // frames with bytes
  float  data[4];    // 4 floating point values
} pack_t ;  

pack_t rdp; // packet to receive


void setup() {
  Serial.begin(9600);
  pinMode(DI0,INPUT);  // signal interrupt                   
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
LoRa.setSpreadingFactor(sf);
LoRa.setSignalBandwidth (sb);
LoRa.setCodingRate4(cr);
}

void loop()  // la boucle de l’emetteur
{
int packetLen;
packetLen=LoRa.parsePacket();
if(packetLen==16)
  {
    i=0;
  while (LoRa.available()) {
    rdp.frame[i]=LoRa.read();i++;
    }
  d1=rdp.data[0];d2=rdp.data[1];
  rssi=LoRa.packetRssi();  // force du signal en réception en dB 
  Serial.println(d1);  Serial.println(d2);
  Serial.println(rssi);
  }
}
