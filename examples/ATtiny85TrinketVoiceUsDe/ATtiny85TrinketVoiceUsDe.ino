// ATtiny85TrinketVoiceUsDe.ino
// 
// Execute powershell to outpout voice.

#include "TrinketHidCombo.h"
#include "iotool_attiny85trinket_util.h"

#define BUILDIN_LED  1   // DigiSpark use PB1 for internal led

void setup() {

  // set pinmode
  pinMode(BUILDIN_LED,OUTPUT);

  // init usb connection (wait until connected)
  digitalWrite(BUILDIN_LED,HIGH);
  TrinketHidCombo.begin();
  while(!TrinketHidCombo.isConnected()) {
    TrinketUtil_delay(10);
  }

  // init keyboard (bugfix: 1000ms+1000ms setup end)
  // note: trinker lib need 2 seconds to init as keyboard!
  digitalWrite(BUILDIN_LED,LOW);
  TrinketHidCombo.pressKey(0,0);
  TrinketUtil_delay(1000);

  // signal setup end
  for (uint8_t i=0;i<5;i++) {
    digitalWrite(BUILDIN_LED,HIGH);
    TrinketUtil_delay(50);
    digitalWrite(BUILDIN_LED,LOW);
    TrinketUtil_delay(150);
  }

}

void script() {

  TrinketHidCombo.println(F("Add-Type -AssemblyName System.speech")); 
  TrinketUtil_delay(500);

  TrinketHidCombo.println(F("$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer")); 
  TrinketUtil_delay(500);

  TrinketHidCombo.println(F("$speak.Speak(\"test\")")); 
  TrinketUtil_delay(1500);

}

void loop() {

  pinMode(BUILDIN_LED,OUTPUT);
  digitalWrite(BUILDIN_LED,HIGH);
  
 // press the Windows Key + R (open "run" command box)
  TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI,KEYCODE_R);
  TrinketUtil_releaseKey();
  TrinketUtil_delay(500);

  // execute powershell
  TrinketHidCombo.print(F("powershell")); 
  TrinketUtil_delay(500);
  TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
  TrinketUtil_releaseKey();
  TrinketUtil_delay(3000);

  // disable CAPSLOCK
  TrinketUtil_capsLockOff();

  // execute script for us keyboard
  TrinketHidCombo.keyboardMapping = KEYBOARD_US_QWERTY;
  script();
  
  // execute script for de keyboard
  TrinketHidCombo.keyboardMapping = KEYBOARD_DE_QWERTZ;
  script();

  // close powershell
  TrinketHidCombo.println(F("exit")); 
  TrinketUtil_delay(500);

  // restore CAPSLOCK
  TrinketUtil_capsLockRestore();

  digitalWrite(BUILDIN_LED,LOW);
  while(1){}

}
