// ATtiny85DigiKeyboardAdcMonitor.ino
// 
// Log analog values of PB0,PB1,PB2 and PB5 to csv or excel.
// tbd: http://21stdigitalhome.blogspot.com/2014/10/trinket-attiny85-internal-temperature.html

#include "DigiKeyboard.h"
// https://cdn.hackaday.io/files/10446446921664/DigiKeyboard_6keys.h
#define KEY_TAB               0x2B     // Keyboard Tab

void setup() {
  pinMode(0,INPUT);
  pinMode(1,INPUT);
  pinMode(2,INPUT);
  pinMode(5,INPUT);
}

static byte state = 0;

void DigiKeyboardNext() {
  DigiKeyboard.delay(100);
}

void loop() {
  if (state == 0) {
    state = 1;
    /*--- init keyboard ---*/
    DigiKeyboard.sendKeyStroke(0);
    /*--- blink buildin led ---*/
    pinMode(1,OUTPUT); 
    digitalWrite(1,HIGH);
    DigiKeyboard.delay(3000);
    digitalWrite(1,LOW);
    pinMode(1,INPUT); 
    DigiKeyboard.delay(1000);
    /*--- start notepad by keyboard ---*/
    DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // Windows+R
    DigiKeyboardNext();
    DigiKeyboard.print("notepad");
    DigiKeyboardNext();
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboardNext();
    DigiKeyboard.delay(3000);
    /*--- header ---*/
    DigiKeyboard.print(F("PB0"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("PB1"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("PB2"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("PB5"));
    // DigiKeyboard.sendKeyStroke(KEY_TAB);
    // DigiKeyboard.print(F("TMP"));
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboardNext();
  }
  /*--- read values ---*/
  uint16_t pb0,pb1,pb2,pb5,tmp;
  pb0 = digitalRead(0);
  pb1 = digitalRead(1);
  pb2 = analogRead(1); // PB2 = ADC1
  pb5 = analogRead(0); // PB5 = ADC0
  // tmp = analogRead(4); // TMP = ADC4
  /*--- display values ---*/
  DigiKeyboard.print(pb0);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  DigiKeyboard.print(pb1);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  DigiKeyboard.print(pb2);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  DigiKeyboard.print(pb5);
  // DigiKeyboard.sendKeyStroke(KEY_TAB);
  // DigiKeyboard.print(tmp);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext();
}
