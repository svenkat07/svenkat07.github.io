#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "access.hpp"
constexpr int SS_PIN=21,RST_PIN=22,RELAY_PIN=26;
MFRC522 reader(SS_PIN,RST_PIN);
access_demo::Controller controller;
void setup(){
 // Active-high relay assumed. Verify module polarity before connecting anything.
 digitalWrite(RELAY_PIN,LOW);pinMode(RELAY_PIN,OUTPUT);
 Serial.begin(115200);SPI.begin(18,19,23,SS_PIN);reader.PCD_Init();
}
void loop(){
 digitalWrite(RELAY_PIN,controller.tick(millis())?HIGH:LOW);
 if(reader.PICC_IsNewCardPresent()&&reader.PICC_ReadCardSerial()){
  char uid[21]{};
  if(reader.uid.size<=10){
   for(byte i=0;i<reader.uid.size;++i)snprintf(uid+2*i,3,"%02X",reader.uid.uidByte[i]);
   auto e=controller.scan(millis(),uid);char payload[256];
   int n=access_demo::json(e,false,payload,sizeof(payload));
   digitalWrite(RELAY_PIN,e.unlocked?HIGH:LOW);
   if(n>0&&n<int(sizeof(payload)))Serial.println(payload);
  }
  reader.PICC_HaltA();reader.PCD_StopCrypto1();
 }
 delay(5);
}
