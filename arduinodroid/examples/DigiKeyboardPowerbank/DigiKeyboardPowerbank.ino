// DigiKeyboardPowerbank.ino
// 
// Detect powerbank by connection timeout
// 
// Add isReady() to DigiKeyboard.h to define v-usb timeout.
// Execute error() if device connect to powerbank or battery.

#include "DigiKeyboard.h"

static uint8_t gKeyboardIsReady;

void setup() {

  // dim led by internal resistor
  pinMode(1,INPUT_PULLUP);
    
  // wait 5 seconds to connect
  uint16_t initTimeout = 5000;
  while (!DigiKeyboard.isReady()) {
    initTimeout -= 5; // delay of isReady
    if (initTimeout == 0) {
      gKeyboardIsReady = 0;
      return;
    }
  }
  gKeyboardIsReady = 1;
    
  // initial keystroke
  DigiKeyboard.sendKeyStroke(0);
  delay(500);
  
  // ready to use
  pinMode(1,OUTPUT);
  digitalWrite(1,HIGH);

}

void error() {
    
  // toggle led to signal error
  pinMode(1,OUTPUT);
  digitalWrite(1,HIGH);
  delay(100);
  digitalWrite(1,LOW);
  delay(100);
    
}

void loop() {

  // on error
  if (!gKeyboardIsReady) {
    error();
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
