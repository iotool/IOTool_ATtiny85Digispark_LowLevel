// DigiKeyboard.ino

#include "DigiKeyboard.h"

void setup() {

  // wait until connected
  pinMode(1,INPUT_PULLUP);
  DigiKeyboard.sendKeyStroke(0);
  delay(2000);
  
  // ready to use
  pinMode(1,OUTPUT);
  digitalWrite(1,HIGH);
}


void loop() {

  DigiKeyboard.print(F("test"));

  // end of script
  digitalWrite(1,LOW);
  while(1) {
    DigiKeyboard.delay(10);
  }

}
