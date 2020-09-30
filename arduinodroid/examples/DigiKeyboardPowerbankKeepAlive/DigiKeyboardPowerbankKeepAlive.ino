// DigiKeyboardPowerbankKeepAlive.ino
// 
// Detect power source by zenner diode.
// If detect powerbank produce > 100mA current for 100ms every 10s.
// To keep the powerbank alive you have to draw more then 100mA current.
// Most powerbank turn off after 15 seconds if there is no high current draw.
// We use a 47 ohm resistor to draw 90mA for 100ms.
//
//                |+5V           current peek
//               [47]  89mA      99mA = 89mA + 10mA
//                |
// PB1 --[470]--|<  N2222        1% = 100ms/10.000ms
//        10mA    |-GND          1mA extra current draw
//
// 
// Add isReady() to DigiKeyboard.h to define v-usb timeout.
// Execute error() if device connect to powerbank or battery.

#include "DigiKeyboard.h"

#define POWER_USB_COMPUTER  1  // led off
#define POWER_USB_POWERBANK 2  // led slow blink
#define POWER_BAT_4V5       3  // led fast tick 1x
#define POWER_BAT_9V0       4  // led fast tick 2x

static uint8_t gPowerSource;

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
  delay(500);
  
  // ready to use
  pinMode(1,OUTPUT);
  digitalWrite(1,HIGH);

}

static int16_t gBatteryTimeout=999999;

void battery() {
    
  // toggle led to signal battery
  switch(gPowerSource) {
    case POWER_USB_POWERBANK:
      // keep alive powerbank
      if (gBatteryTimeout >= 10000) {
        gBatteryTimeout = 0;
        pinMode(1,OUTPUT); // 93mA
        digitalWrite(1,HIGH);
        pinMode(3,OUTPUT); // 4mA
        digitalWrite(3,LOW);
        pinMode(4,OUTPUT); // 26mA
        digitalWrite(1,HIGH);
        delay(100); gBatteryTimeout += 100;
        digitalWrite(1,LOW);
        pinMode(3,INPUT);
        digitalWrite(4,LOW);
        delay(900); gBatteryTimeout += 900;
      }
      // blink dimmed test
      pinMode(1,OUTPUT);
      digitalWrite(1,HIGH);
      delay(10); gBatteryTimeout += 10;
      digitalWrite(1,LOW);
      delay(990); gBatteryTimeout += 990;
      break;
    case POWER_BAT_9V0:
      pinMode(1,INPUT_PULLUP);
      delay(100);
      pinMode(1,OUTPUT);
      digitalWrite(1,LOW);
      delay(100);
    case POWER_BAT_4V5:
      pinMode(1,INPUT_PULLUP);
      delay(100);
      pinMode(1,OUTPUT);
      digitalWrite(1,LOW);
      delay(900);
      break;
  }

    
}

void loop() {

  // on error
  if (gPowerSource != POWER_USB_COMPUTER) {
    battery();
    return;
  }
  
  // send keystrokes
  DigiKeyboard.print(F("test"));

  // end of script
  digitalWrite(1,LOW);
  while(1) {
    DigiKeyboard.delay(10);
  }

}
