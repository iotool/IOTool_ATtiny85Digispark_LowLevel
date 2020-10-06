// DigiKeyboardDataLogger.ino
// 
// Save analogRead of PB2 every 10 seconds to EEPROM (battery).
// Send 408 logs from EEPROM as keyboard input (computer).
// Use 10-bit unsigned int for adc values.
// 
// Add isReady() to DigiKeyboard.h to define v-usb timeout.

#include "DigiKeyboard.h"
#include <avr/eeprom.h>

#define POWER_USB_COMPUTER  1  // led off
#define POWER_USB_POWERBANK 2  // led slow blink
#define POWER_BAT_4V5       3  // led fast tick 1x
#define POWER_BAT_9V0       4  // led fast tick 2x

static uint8_t gPowerSource;

typedef struct {
  unsigned index:9;
  unsigned :7;
} tEepromHead;
tEepromHead gEepromHead __attribute__
  ((__section__ (".noinit")))
;

typedef struct {
  unsigned value_0:10;
  unsigned value_1:10;
  unsigned value_2:10;
  unsigned value_3:10;
} tEepromData;
tEepromData gEepromData __attribute__
  ((__section__ (".noinit")))
;

#define EEPROM_LOGS_TIMEOUT  10000
#define EEPROM_HEAD_OFFSET   511
#define EEPROM_HEAD_BYTES    2
#define EEPROM_HEAD_READ     eeprom_read_block((void*)&gEepromHead, (const void*)EEPROM_HEAD_OFFSET, EEPROM_HEAD_BYTES)
#define EEPROM_HEAD_WRITE    eeprom_write_block(&gEepromHead, (void*)EEPROM_HEAD_OFFSET, EEPROM_HEAD_BYTES)
#define EEPROM_DATA_VALUES   408
#define EEPROM_DATA_BYTES    5
#define EEPROM_DATA_READ     eeprom_read_block((void*)&gEepromData, (const void*)gEepromDataBlock, EEPROM_DATA_BYTES)
#define EEPROM_DATA_WRITE    eeprom_write_block(&gEepromData, (void*)gEepromDataBlock, EEPROM_DATA_BYTES)

static uint16_t gEepromDataBlock;
static uint16_t gEepromDataValue;
static uint16_t gEepromLogsTimeout = 0;

uint16_t getEepromData(uint16_t pIndex) {
  uint16_t lData = 0xFFFF;
  gEepromDataBlock = 0;
  gEepromDataValue = pIndex;
  if (gEepromDataValue >= EEPROM_DATA_VALUES) {
    gEepromDataValue = 0;
  }
  while (gEepromDataValue >= 4) {
    gEepromDataValue -=4 ;
    gEepromDataBlock += EEPROM_DATA_BYTES;
  }
  EEPROM_DATA_READ;
  switch(gEepromDataValue) {
    case 0: lData = gEepromData.value_0; break;
    case 1: lData = gEepromData.value_1; break;
    case 2: lData = gEepromData.value_2; break;
    case 3: lData = gEepromData.value_3; break;
  }
  return lData;
}

void setEepromData(uint16_t pIndex, uint16_t pData) {
  uint16_t lData = getEepromData(pIndex);
  if (lData != pData) {
    switch(gEepromDataValue) {
      case 0: gEepromData.value_0 = pData; break;
      case 1: gEepromData.value_1 = pData; break;
      case 2: gEepromData.value_2 = pData; break;
      case 3: gEepromData.value_3 = pData; break;
    }
    EEPROM_DATA_WRITE;
  }
}

void setup() {

  // dim led by internal resistor
  pinMode(1,INPUT_PULLUP);
    
  // wait 5 seconds to connect
  uint16_t initTimeout = 5000;
  while (!DigiKeyboard.isReady()) {
    initTimeout -= 5; // delay of isReady
    if (initTimeout == 0) {
      pinMode(3,INPUT);
      initTimeout += analogRead(3);
      initTimeout += analogRead(3);
      initTimeout += analogRead(3);
      initTimeout += analogRead(3);
      initTimeout = initTimeout >> 2;
      if (initTimeout > 636) {
        // ADC=660 (PB3~2.93V 5V~4.54V)        
        gPowerSource = POWER_BAT_4V5;
      }      
      else if (initTimeout > 601) {
        // ADC=613 (PB3~3.01V 5V~5.02V)
        gPowerSource = POWER_BAT_9V0;
      }
      else { // initTimeout <= 601
        // ADC=591 (PB3~2.78V 5V~4.81V)
        gPowerSource = POWER_USB_POWERBANK;
      }
      return;
    }
  }
  gPowerSource = POWER_USB_COMPUTER;
    
  // initial keystroke
  DigiKeyboard.sendKeyStroke(0);
  delay(2000);
  
  // ready to use
  pinMode(1,OUTPUT);
  digitalWrite(1,HIGH);

}

void battery() {
    
  // toggle led to signal battery
  pinMode(1,OUTPUT);
  switch(gPowerSource) {
    case POWER_USB_POWERBANK:
      digitalWrite(1,HIGH);
      delay(500);
      digitalWrite(1,LOW);
      delay(500);
      break;
    case POWER_BAT_9V0:
      digitalWrite(1,HIGH);
      delay(10);
      digitalWrite(1,LOW);
      delay(90);
      digitalWrite(1,HIGH);
      delay(10);
      digitalWrite(1,LOW);
      delay(890);
    case POWER_BAT_4V5:
      digitalWrite(1,HIGH);
      delay(10);
      digitalWrite(1,LOW);
      delay(990); 
      break;
  }

  // write adc value to eeprom
  gEepromLogsTimeout += 1000;
  if (gEepromLogsTimeout >= EEPROM_LOGS_TIMEOUT) {
    gEepromLogsTimeout = 0;
    pinMode(2,INPUT);
    uint16_t lIndex,lData;
    gEepromHead.index++;
    if (gEepromHead.index >= EEPROM_DATA_VALUES) {
      gEepromHead.index = 0;
    }
    lIndex = gEepromHead.index;
    lData = analogRead(1); // PB2 = ADC1;
    setEepromData(lIndex, lData);
    EEPROM_HEAD_WRITE;
  
  }

}

void computer() {

  uint16_t lIndex;
  DigiKeyboard.println(F("HEAD"));
  EEPROM_HEAD_READ;
  lIndex = gEepromHead.index;
  DigiKeyboard.println(lIndex);

  // read adc values from eeprom
  DigiKeyboard.println(F("DATA"));
  uint16_t lData;
  for (uint16_t i=0; i<EEPROM_DATA_VALUES; i++) {
    lData = getEepromData(i);
    DigiKeyboard.print(i);
    DigiKeyboard.print(" ");
    DigiKeyboard.println(lData);
  }
  
}

void loop() {

  // usb timeout
  if (gPowerSource != POWER_USB_COMPUTER) {
    battery();
    return;
  }

  // usb keyboard
  computer();
  
  // end of script
  digitalWrite(1,LOW);
  while(1) {
    DigiKeyboard.delay(10);
  }

}
