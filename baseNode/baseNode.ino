#include <JeeLib.h>
#include <avr/eeprom.h>
#include "NodeProtocol.h"

#define DEBUG  1

MilliTimer sendTimer;

typedef struct {
    byte nodeId;
    byte group;
    char msg[RF12_EEPROM_SIZE-4];
    word crc;
} RF12Config;

Payload inData, outData;
RF12Config config;

byte pendingOutput;
char line[25];

void setup () {
  Serial.begin(57600);
  Serial.print("[BaseNode.v1.1]");
  

  if (rf12_config()) {
    config.nodeId = eeprom_read_byte(RF12_EEPROM_ADDR);
    config.group = eeprom_read_byte(RF12_EEPROM_ADDR + 1);
  } else {
     config.nodeId = 0x41; // node A1 @ 433 MHz
     config.group = 0xD4;
     //saveConfig();
  }
}

static void consumeInData () {
        Serial.print("ROOM ");

}

static byte produceOutData () {
    return 0;
}

void loop () {
  
    
    //if (rf12_recvDone() && rf12_crc == 0 && rf12_len == sizeof inData) {
    if (rf12_recvDone() && rf12_crc == 0) {
      
        memcpy(&inData, (byte*) rf12_data, sizeof inData);
        // optional: rf12_recvDone(); // re-enable reception right away
        #if DEBUG
          Serial.print("DEBUG ");
          Serial.print((int) rf12_hdr);
          for (byte i = 0; i < rf12_len; ++i) {
            Serial.print(' ');
            Serial.print((int) rf12_data[i]);
          }
          Serial.println();
        #endif
        
        Serial.print("OK ");
        Serial.print(rf12_hdr & 0x1F);
        Serial.print(' ');
        Serial.print(inData.moved, DEC);
        Serial.print(' ');
        Serial.print(inData.temp, DEC);
        Serial.print(' ');
        Serial.print(inData.humi, DEC);
        Serial.print(' ');
        Serial.print(inData.light, DEC);
        Serial.print(' ');
        Serial.println(inData.lobat, DEC);
    }

    if (sendTimer.poll(100))
        pendingOutput = produceOutData();

    if (pendingOutput && rf12_canSend()) {
        rf12_sendStart(0, &outData, sizeof outData, 2);
        // optional: rf12_sendWait(2); // wait for send to finish
        pendingOutput = 0;
    }
}
