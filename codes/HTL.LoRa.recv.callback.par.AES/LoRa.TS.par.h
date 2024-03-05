#include <EEPROM.h>

typedef union
{
  uint8_t buff[36];
  struct 
  {
    uint32_t  channel;            // thingspeak channel number for Gateway
    char      wkey[16];           // thingspeak write key
    char      rkey[16];           // thingspeak read key
  } par;
} ts_t;

typedef union
{
  uint8_t buff[28];
  struct
  {
    uint32_t  freq;         // GW - link frequency
    uint8_t   power;        // GW - emission power
    uint8_t   sf;           // GW - link spreading factor
    uint32_t  bw;           // GW - link signal bandwidth: [125E3, 250E3,500E3]
    uint8_t   cr;           // GW - link coding rate: [5,6,6,8] in function
    uint8_t   aeskey[16];   // GW - AES key
    uint8_t   pad;
  } par;
} lora_t;


void readEEPROM(ts_t *ts,lora_t *lora)
{
EEPROM.begin(512); delay(100);
Serial.println("Start to read from EEPROM");
for(int j=0;j<36;j++) ts->buff[j]=EEPROM.read(j+136);
for(int k=0;k<28;k++) lora->buff[k]=EEPROM.read(k+172);
delay(100);
}
