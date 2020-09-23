// ATtiny85DigiKeyboardMouse.ino (work in progress - does not work)
//
// Normaly both libs can not use at the same sketch. 
// The idea is to toggle the device type after software reboot.
// Keyboard is working fine after reboot, but the mouse do nothing.
// 
// tbd: usbFunctionDescriptor - to get this running the usbFunctionDescriptor 
// at DigiKeyboardMouse.h is required (comment because of compile errors)
// 
// digistump\hardware\avr\1.6.7\libraries\DigisparkKeyboard\DigiKeyboardMouse.h
// sketch\DigiKeyboardMouseCombi\DigiSleep.h
//
// Append DigiKeyboardMouse.h to the digistump folder at DigiKeyboard library.
// Append DigiSleep.h as new sketch tab, if Arduino IDE has a error with sleep.h
//

#define __AVR_ATtiny85__         // Arduino 1.8.2 workaround
#include "DigiSleep.h"           // Arduino 1.8.2 workaround
#include "DigiKeyboardMouse.h"   // combined library for keyboard and mouse

#define BUILDIN_LED 1

void setup() {
  pinMode(BUILDIN_LED,OUTPUT);

  // init memory to save flag of mode (keyboard, mouse)
  initRebootMemory();

  // setup device if act as mouse
  if (gRebootMemory.mode == REBOOT_MODE_MOUSE) {
    gRebootMemory.next = REBOOT_MODE_KEYBOARD;
    DigiMouse.begin();
    // blink short = mouse
    for (byte i=0; i<10; i++) {
      digitalWrite(BUILDIN_LED,HIGH);
      delayKeyboardMouse(10);
      digitalWrite(BUILDIN_LED,LOW);
      delayKeyboardMouse(90);
    }
  } 
  
  // setup device if act as keyboard
  else {
    gRebootMemory.next = REBOOT_MODE_MOUSE;
    DigiKeyboard.sendKeyStroke(0,0);
    // blink long = keyboard
    for (byte i=0; i<5; i++) {
      digitalWrite(BUILDIN_LED,HIGH);
      delayKeyboardMouse(50);
      digitalWrite(BUILDIN_LED,LOW);
      delayKeyboardMouse(150);
    }
  }
}

void loop() {

  // keep alive as usb device (keyboard, mouse)
  delayKeyboardMouse(1000);

  // act as mouse 
  if (gRebootMemory.mode == REBOOT_MODE_MOUSE) {
    DigiMouse.update();
    DigiMouse.moveY(100);
    delayKeyboardMouse(500);
    DigiMouse.moveX(200);
    delayKeyboardMouse(500);
    DigiMouse.scroll(5);
    delayKeyboardMouse(500);
    DigiMouse.update();
  } 
  
  // act as keyboard
  else {
    DigiKeyboard.update();
    DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // Windows+R
    delayKeyboardMouse(1500);
  }  

  // end of action
  delayKeyboardMouse(1000);
  digitalWrite(BUILDIN_LED,HIGH);
  delayKeyboardMouse(2000);
  digitalWrite(BUILDIN_LED,LOW);

  // reboot bootloader and toggle mode
  reboot();
  while(1){}
}
