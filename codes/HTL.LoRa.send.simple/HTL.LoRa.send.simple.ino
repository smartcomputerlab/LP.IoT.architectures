#include <SPI.h>              // include libraries
#include <LoRa.h>
#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    59   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS (NSS)
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI0     26   // GPIO26 -- SX127x's IRQ(DIO0)
#define freq    868E6   
#define sf 7
#define sb 125E3       
#define cr 8   

typedef union 
{
  uint8_t frame[16]; // frames with bytes
  float  data[4];    // 4 floating point values
} pack_t ;  

pack_t sdp; // packet to send

void setup() {
  Serial.begin(9600);delay(1000);
  Serial.println(); Serial.println();
  pinMode(DI0,INPUT);  // signal interrupt                   
  SPI.begin(SCK,MISO,MISO,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  else Serial.println("Starting LoRa OK!");
LoRa.setSpreadingFactor(sf);
LoRa.setSignalBandwidth(sb);
LoRa.setCodingRate4(cr);
}


float d1=12.0, d2=321.54 ;

void loop()  // la boucle de l’emetteur
{
Serial.println("New Packet") ;
  LoRa.beginPacket();                   // start packet
  sdp.data[0]=d1;
  sdp.data[1]=d2;
  LoRa.write(sdp.frame,16);
  LoRa.endPacket();
d1++; d2+=2;
delay(2000);
}
