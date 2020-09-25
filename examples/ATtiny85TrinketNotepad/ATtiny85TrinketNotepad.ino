// ATtiny85TrinketNotepad.ino
// 
// HID emulation of a keyboard and mouse with DigiSpark ATtiny85.
// Example of using the keyboard, keyboard led and mouse within one sketch.
// 
// After plugging into the USB port, the three LEDs on the keyboard will flash. 
// The mouse is moved 50 pixels to the lower right.
// Via Windows open dialog the application Notepad is started.
// A header line for CSV is output with PB0, PB2, PB5 and state of NUMLOCK.
// Every second, data is output separated by tabulator.
// Press NUMLOCK to log data every 100 milli seconds.
// 
// Trinker's library is much more powerful than DigiStump's.
// Keyboard and mouse can be used in a sketch. Furthermore the status of the LEDs 
// for CAPSLOCK, NUMLOCK and SCROLLOCK can be queried.
// 
// Notes:
// - The USB communication must be initialized for 2 seconds before the keyboard is stable.
// - Each keystroke must be released via key code 0 (see TrinketUtil_releaseKey).
// - With CAPSLOCK switched on, special characters are printed with PRINT instead of numbers. 
// - Inputs to virtual containers are sometimes output in the wrong order (use USBWAIT). 
// 

#include "TrinketHidCombo.h"
#include "iotool_attiny85trinket_util.h"

#define BUILDIN_LED  1   // DigiSpark use PB1 for internal led
#define PB0          0   // free pin
#define PB2          2   // free pin
#define PB5          3   // reset pin
#define ADC0         0   // adc0 = pb5
#define ADC1         1   // adc1 = pb2
#define USBWAIT      10  // delay for flush

void setup() {

  // set pinmode
  pinMode(PB0,INPUT);
  pinMode(PB2,INPUT);
  pinMode(PB5,INPUT);
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

  // wait to switch to excel
  TrinketUtil_delay(3000);

  // csv header
  TrinketUtil_capsLockOff();                 TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.print(F("PB0"));           TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.pressKey(0,KEYCODE_TAB);   TrinketUtil_delay(USBWAIT);
  TrinketUtil_releaseKey();                  TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.print(F("PB2"));           TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.pressKey(0,KEYCODE_TAB);   TrinketUtil_delay(USBWAIT);
  TrinketUtil_releaseKey();                  TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.print(F("PB5"));           TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.pressKey(0,KEYCODE_TAB);   TrinketUtil_delay(USBWAIT);
  TrinketUtil_releaseKey();                  TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.println(F("NUMLOCK"));     TrinketUtil_delay(USBWAIT);
  TrinketUtil_capsLockRestore();             TrinketUtil_delay(USBWAIT);
  TrinketUtil_delay(500);

}

void loop() {

  // led for CAPSLOCK
  if ((TrinketHidCombo.getLEDstate() & KB_LED_CAPS) == KB_LED_CAPS) {
    digitalWrite(BUILDIN_LED,HIGH);
  } else {
    digitalWrite(BUILDIN_LED,LOW);
  }

  // read data
  uint16_t pb0read, pb2read, pb5read, numlock;
  pb0read = digitalRead(PB0);
  pb2read = analogRead(ADC1);
  pb5read = analogRead(ADC0);
  numlock = ((TrinketHidCombo.getLEDstate() & KB_LED_NUM) == KB_LED_NUM);

  // csv data
  TrinketUtil_capsLockOff();                 TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.print(pb0read);            TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.pressKey(0,KEYCODE_TAB);   TrinketUtil_delay(USBWAIT);
  TrinketUtil_releaseKey();                  TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.print(pb2read);            TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.pressKey(0,KEYCODE_TAB);   TrinketUtil_delay(USBWAIT);
  TrinketUtil_releaseKey();                  TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.print(pb5read);            TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.pressKey(0,KEYCODE_TAB);   TrinketUtil_delay(USBWAIT);
  TrinketUtil_releaseKey();                  TrinketUtil_delay(USBWAIT);
  TrinketHidCombo.println(numlock);          TrinketUtil_delay(USBWAIT);
  TrinketUtil_capsLockRestore();             TrinketUtil_delay(USBWAIT);

  // slow or fast messure by numlock key
  if (numlock == 0) {
    TrinketUtil_delay(910);
  } else {
    TrinketUtil_delay(10);
  }

  //- // power down computer after 90 seconds
  //- if (millis() > 90000) {
  //-   TrinketHidCombo.pressSystemCtrlKey(SYSCTRLKEY_SLEEP);
  //-   TrinketUtil_releaseKey();
  //-   while(1) {
  //-     TrinketUtil_delay(10);
  //-   }
  //- }

}
