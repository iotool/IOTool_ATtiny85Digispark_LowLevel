//  ATtiny85TrinketHidComboLED.ino
// 
//  https://github.com/adafruit/Adafruit-Trinket-USB/tree/master/TrinketHidCombo
//  Download Adafruit-Trinket-USB-master.zip and extract files.
//  Copy TrinketHidCombo\*.* to C:\Users\xyz\Documents\Arduino\libraries
//  Start Arduino IDE 1.6.8 and compile example TrinketVolumeKnob.
// 

#include "TrinketHidCombo.h"
#define BUILDIN_LED 1

void delayTrinket(uint16_t ms);

uint8_t ledStatePrev;

void setup() {
  // buildin led on
  pinMode(BUILDIN_LED,OUTPUT);
  digitalWrite(BUILDIN_LED,HIGH);
  // start the USB device engine and enumerate
  TrinketHidCombo.begin();
  // keep buildin led on, if not connected
  while(!TrinketHidCombo.isConnected()) {
    delayTrinket(100);
  }
  // buildin led off, if connected
  digitalWrite(BUILDIN_LED,LOW);
  delayTrinket(900); 
  ledStatePrev = TrinketHidCombo.getLEDstate();
}

void loop() {
  // returns the state of the three LEDs on a keyboard (caps/num/scroll lock)
  uint8_t ledState = TrinketHidCombo.getLEDstate();

  // blink on state changed
  if (ledState != ledStatePrev) {
    ledStatePrev = ledState;
    if ( (ledState & KB_LED_NUM) == KB_LED_NUM) {
      digitalWrite(BUILDIN_LED,LOW);
      delayTrinket(1000);
      for (byte i=1;i<=1;i++) {
        digitalWrite(BUILDIN_LED,HIGH);
        delayTrinket(50);
        digitalWrite(BUILDIN_LED,LOW);
        delayTrinket(200);
      }
    }
    if ( (ledState & KB_LED_CAPS) == KB_LED_CAPS) {
      digitalWrite(BUILDIN_LED,LOW);
      delayTrinket(1000);
      for (byte i=1;i<=2;i++) {
        digitalWrite(BUILDIN_LED,HIGH);
        delayTrinket(50);
        digitalWrite(BUILDIN_LED,LOW);
        delayTrinket(200);
      }
    }
    if ( (ledState & KB_LED_SCROLL) == KB_LED_SCROLL) {
      digitalWrite(BUILDIN_LED,LOW);
      delayTrinket(1000);
      for (byte i=1;i<=3;i++) {
        digitalWrite(BUILDIN_LED,HIGH);
        delayTrinket(50);
        digitalWrite(BUILDIN_LED,LOW);
        delayTrinket(200);
      }
    }
    delayTrinket(1000);
  }

  // led on / off
  if ( ledState != 0) {
    digitalWrite(BUILDIN_LED,HIGH);
  } else {
    digitalWrite(BUILDIN_LED,LOW);
  }

  // noop
  delayTrinket(100); 
}

void delayTrinket(uint16_t ms) {
  // poll() must be called at least once every 10ms
  while (ms>10) {
    TrinketHidCombo.poll(); 
    ms-=10;
    delay(10);
  }
  TrinketHidCombo.poll(); 
  delay(ms);
}
