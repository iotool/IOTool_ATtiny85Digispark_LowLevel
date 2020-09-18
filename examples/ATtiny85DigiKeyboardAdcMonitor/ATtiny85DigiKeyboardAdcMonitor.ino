// ATtiny85DigiKeyboardAdcMonitor.ino
// 
// Log analog values of ADC PB2 to csv or excel.

#include "DigiKeyboard.h"

void setup() {
  pinMode(1,OUTPUT);
  pinMode(2,INPUT);
}

void DigiKeyboardNext() {
  digitalWrite(1,LOW);
  DigiKeyboard.delay(100);
  digitalWrite(1,HIGH);
}

void loop() {
  digitalWrite(1,HIGH);
  /*--- init keyboard ---*/
  DigiKeyboard.sendKeyStroke(0);
  DigiKeyboard.delay(4000);
  /*--- open execute dialog ---*/
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // Windows+R
  DigiKeyboardNext();
  /*--- run powershell ---*/
  DigiKeyboard.print("notepad");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext();
  DigiKeyboard.delay(5000);
  /*--- header ---*/
  DigiKeyboard.print("PB2");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext();
  uint16_t pb2;
  while(1) {
    pb2 = analogRead(1); // PB2 = ADC1
    DigiKeyboard.print(pb2);
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboardNext();
  }
  digitalWrite(1,LOW);
  for (;;) {/*empty*/}
}
