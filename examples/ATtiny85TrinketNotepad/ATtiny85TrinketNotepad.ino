// ATtiny85TrinketNotepad.ino
// 
// HID emulation of a keyboard and mouse with DigiSpark ATtiny85.
// Example of using the keyboard, keyboard led and mouse within one sketch.
// 
// After plugging into the USB port, the three LEDs on the keyboard will flash. 
// The mouse is moved 50 pixels to the lower right.
// Via Windows open dialog the application Notepad is started.
// A header line for CSV is output with PB0, PB2 and state of NUMLOCK.
// Every second, data is output separated by TAB.
// 
// Trinker's library is much more powerful than DigiStump's.
// Keyboard and mouse can be used in a sketch. Furthermore the status of the LEDs 
// for CAPSLOCK, NUMLOCK and SCROLLOCK can be queried.
// 
// Notes:
// - The USB communication must be initialized for 2 seconds before the keyboard is stable.
// - Each keystroke must be released via key code 0 (see TrinketUtil_releaseKey).
// - With CAPSLOCK switched on, special characters are printed with PRINT instead of numbers. 
// 
// 

#include "TrinketHidCombo.h"
#include "iotool_attiny85trinket_util.h"

#define BUILDIN_LED  1  // DigiSpark use PB1 for internal led
#define PB0          0  // free pin
#define PB2          2  // free pin
#define ADC0         0  // adc0 = pb2

void setup() {

  // set pinmode
  pinMode(PB0,INPUT);
  pinMode(PB2,INPUT);
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

  // init and move mouse
  TrinketHidCombo.mouseMove(50,50,0);
  TrinketUtil_delay(100);

  // blink physical led at keyboard
  for (uint8_t i=0;i<4;i++) {
    TrinketHidCombo.pressKey(0,KEYCODE_CAPSLOCK);
    TrinketUtil_releaseKey();
    TrinketHidCombo.pressKey(0,KEYCODE_NUMLOCK);
    TrinketUtil_releaseKey();
    TrinketHidCombo.pressKey(0,KEYCODE_SCROLLLOCK);
    TrinketUtil_releaseKey();
    TrinketUtil_delay(250);
  }

  // press the Windows Key + R (open "run" command box)
  TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI,KEYCODE_R);
  TrinketUtil_releaseKey();
  TrinketUtil_delay(500);

  // execute notepad
  TrinketHidCombo.print(F("notepad")); 
  TrinketUtil_delay(500);
  TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
  TrinketUtil_releaseKey();
  TrinketUtil_delay(500);

  // csv header
  TrinketUtil_capsLockOff();
  TrinketHidCombo.print(F("PB0"));
  TrinketHidCombo.pressKey(0,KEYCODE_TAB); 
  TrinketUtil_releaseKey();
  TrinketHidCombo.print(F("PB2")); 
  TrinketHidCombo.pressKey(0,KEYCODE_TAB); 
  TrinketUtil_releaseKey();
  TrinketHidCombo.println(F("NUMLOCK")); 
  TrinketUtil_capsLockRestore();
  TrinketUtil_delay(500);

}

void loop() {

  // keep usb connection
  TrinketUtil_delay(1000);

  // led for CAPSLOCK
  if ((TrinketHidCombo.getLEDstate() & KB_LED_CAPS) == KB_LED_CAPS) {
    digitalWrite(BUILDIN_LED,HIGH);
  } else {
    digitalWrite(BUILDIN_LED,LOW);
  }

  // read data
  uint16_t pb0read, pb2read, numlock;
  pb0read = digitalRead(PB0);
  pb2read = analogRead(ADC0);
  numlock = ((TrinketHidCombo.getLEDstate() & KB_LED_NUM) == KB_LED_NUM);

  // csv data
  TrinketUtil_capsLockOff();
  TrinketHidCombo.print(pb0read);
  TrinketHidCombo.pressKey(0,KEYCODE_TAB); 
  TrinketUtil_releaseKey();
  TrinketHidCombo.print(pb2read); 
  TrinketHidCombo.pressKey(0,KEYCODE_TAB); 
  TrinketUtil_releaseKey();
  TrinketHidCombo.println(numlock); 
  TrinketUtil_capsLockRestore();
  TrinketUtil_delay(100);

  //- // power down computer after 90 seconds
  //- if (millis() > 90000) {
  //-   TrinketHidCombo.pressSystemCtrlKey(SYSCTRLKEY_SLEEP);
  //-   TrinketUtil_releaseKey();
  //-   while(1) {
  //-     TrinketUtil_delay(10);
  //-   }
  //- }

}
