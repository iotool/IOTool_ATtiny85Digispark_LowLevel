// DigiKeyboardUsDe_Powershell.ino
// Example powershell script for US and DE layout.

#include "DigiKeyboard.h"      // qwerty
#include "DigiKeyboardDe.h"    // qwertz
//#include "DigiKeyboardFr.h"  // awerty
#include "DigiKeyboardUtil.h"  // util

#define BUILDIN_LED 1

void setup() {
  DigiKeyboard.sendKeyStroke(0,0);
  DigiKeyboard.delay(4000);
}

void enter() {
  DigiKeyboard.sendKeyStroke(KEY_ENTER,0);
  DigiKeyboard.delay(500);
  DigiKeyboard.update();
}

void script() {
  String cmd;
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT);  // Windows+R
  DigiKeyboard.delay(1000);
  DigiKeyboard_print("powershell"); enter();
  cmd="Add-Type -AssemblyName System.speech"; DigiKeyboard_print(cmd); enter();
  cmd="$speak = New-Object System.Speech.Sy"; DigiKeyboard_print(cmd); 
             cmd="nthesis.SpeechSynthesizer"; DigiKeyboard_print(cmd); enter();
  cmd="$speak.Speak(\"test\")"; DigiKeyboard_print(cmd); enter();
  DigiKeyboard.delay(1000);
  DigiKeyboard_print("exit"); enter();
}

void loop() {
  pinMode(BUILDIN_LED,OUTPUT);
  digitalWrite(BUILDIN_LED,HIGH);
  DigiKeyboard_setKeyboardLayout(KEYBOARD_LAYOUT_US); script();
  DigiKeyboard_setKeyboardLayout(KEYBOARD_LAYOUT_DE); script();
  //for(uint8_t layout=1; layout<=3; layout++) {
  //  DigiKeyboard_setKeyboardLayout(layout);
  //  script();
  //}
  digitalWrite(BUILDIN_LED,LOW);
  while(1){}
}
