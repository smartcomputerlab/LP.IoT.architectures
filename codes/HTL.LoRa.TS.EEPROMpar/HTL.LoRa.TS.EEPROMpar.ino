#include <EEPROM.h>
uint8_t aeskey[16]={0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70};


typedef union
{
uint8_t buff[36];
struct 
  {
  uint32_t channel; // thingspeak channel number for Gateway
  char wkey[16]; // thingspeak write key
  char rkey[16]; // thingspeak read key
  } par;
} ts_par_t;
ts_par_t tspar;

typedef union
{
uint8_t buff[28];
struct
  {
  uint32_t freq; // GW - link frequency
  uint8_t power; // GW - emission power
  uint8_t sf; // GW - link spreading factor
  uint32_t bw; // GW - link signal bandwidth: [125E3, 250E3,500E3]
  uint8_t cr; // GW - link coding rate: [5,6,6,8] in function
  uint8_t aeskey[16]; // GW - AES key
  uint8_t pad;
  } par;
} gw_lora_t;
gw_lora_t gwlora;

void readEEPROM(ts_par_t *ts, gw_lora_t *lora)
{
EEPROM.begin(512); delay(100);
Serial.println("Start to read from EEPROM");
for(int j=0;j<36;j++) ts->buff[j]=EEPROM.read(j+136);
for(int k=0;k<28;k++) lora->buff[k]=EEPROM.read(k+172);
delay(100);
}

void setup() 
{
Serial.begin(9600);delay(100);
Serial.println("");
Serial.println(); Serial.println("Start loading EEPROM with ap_serv par");
tspar.par.channel=1538804;
strncpy(tspar.par.wkey,"YOX31M0EDKO0JATK",16);
Serial.println(); Serial.println("Start loading EEPROM with LoRa par");
gwlora.par.freq = 868500000;
gwlora.par.power= 14;
gwlora.par.sf=9;
gwlora.par.bw=125E3;
gwlora.par.cr=5; // 4/5 
memcpy(gwlora.par.aeskey,aeskey,16);
Serial.println("Writing to EEPROM");
EEPROM.begin(512); delay(100);
for(int j=0;j<36;j++) EEPROM.write(j+136, tspar.buff[j]);
for(int k=0;k<28;k++) EEPROM.write(k+172, gwlora.buff[k]);
if(EEPROM.commit()) Serial.printf("EEPROM succes\n");
delay(1000);EEPROM.end();delay(1000);
delay(1000);
Serial.printf("Parameters written to EEPROM !\n");
}

void loop() {
readEEPROM(&tspar,&gwlora);
Serial.printf("TS channel: %d\n",tspar.par.channel); 
Serial.printf("TS write key: %16.16s\n",tspar.par.wkey);
Serial.printf("frequency: %d\n",gwlora.par.freq); 
Serial.printf("bandwidth: %d\n",gwlora.par.bw); 
Serial.printf("spreading factor: %d\n",gwlora.par.sf); 
Serial.printf("coding rate: %d/8\n",gwlora.par.cr); 
Serial.print("AES key (HEX): ");
for(int i=0; i<16; i++) Serial.print(gwlora.par.aeskey[i],HEX);
Serial.println();
Serial.printf("AES key (ASCII): %16.16s \n",gwlora.par.aeskey);
delay(10000);
}
