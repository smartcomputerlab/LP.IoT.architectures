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


typedef union 
{
  uint8_t frame[16]; // frames with bytes
  float  data[4];    // 4 floating point values
} pack_t ;  

QueueHandle_t pqueue;   // packet queue

void onReceive(int packetSize) 
{ 
int i=0;
pack_t rdp; // packet to receive

  if(packetSize==0) return;   // if there's no packet, return 
  i=0; 
  if (packetSize==16) 
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
  ...begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
LoRa.setSpreadingFactor(sf);
LoRa.setSignalBandwidth (sb);
LoRa.setCodingRate4(cr);
pqueue = xQueueCreate(4,16);  // sizeof rdp union 
LoRa...(onReceive);  // link to ISR
LoRa.receive();             // reception activation
}

void loop()  // la boucle de l’emetteur
{
int packetLen;
pack_t rdp;
  xQueueReceive(pqueue,&rdp,portMAX_DELAY); 
  Serial.println("data received from the queue");   
  Serial.println(rdp.data[0]);  Serial.println(rdp.data[1]);
}
