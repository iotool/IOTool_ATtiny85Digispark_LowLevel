// iotool_attiny85trinket_util.h
// 
// Utility for library TrinketHidCombo.h as replacement 
// for DigiKeyboard and DigiMouse.
// 
// https://github.com/iotool/IOTool_ATtiny85Digispark_LowLevel
// https://github.com/adafruit/Adafruit-Trinket-USB/tree/master/TrinketHidCombo
// 
// TrinketHidCombo.begin();              // starts the USB driver
// TrinketHidCombo.poll();               // must be called at least once every 10ms
// TrinketHidCombo.isConnected();        // checks if USB is connected, 0 if not connected
// TrinketHidCombo.mouseMove(signed char x, signed char y, uint8_t buttonMask);
// TrinketHidCombo.print(char*);         // print string
// TrinketHidCombo.println(char*);       // print string with line feed
// TrinketHidCombo.pressKey(uint8_t modifiers, uint8_t keycode1[, keycode2..keycode5]);
// TrinketHidCombo.pressMultimediaKey(); // MMKEY_VOL_UP, MMKEY_VOL_DOWN
// TrinketHidCombo.pressSystemCtrlKey(); // SYSCTRLKEY_SLEEP
// 
// TrinketUtil_delay(uint16_t millis);   // keep usb connection during pause
// TrinketUtil_releaseKey();             // end keystroke after pressKey
// TrinketUtil_capsLockOff();            // disable capslock before print
// TrinketUtil_capsLockRestore();        // restore capslock after print
// 
// 2020-09-25  RoHa  v1.0 delay, releaseKey

#ifndef IOTOOL_ATTINY85TRINKET_UTIL_H_
#define IOTOOL_ATTINY85TRINKET_UTIL_H_

#define KEYCODE_CAPSLOCK       0x39 // 57
#define KEYCODE_SCROLLLOCK     0x47 // 71
#define KEYCODE_NUMLOCK        0x53 // 83

void TrinketUtil_delay(uint16_t ms) {
  // poll() must be called at least once every 10ms to keep up 
  // the communication between microcontroller and computer via USB.
  while (ms>10) {
    TrinketHidCombo.poll(); 
    ms-=10;
    delay(10);
  }
  TrinketHidCombo.poll(); 
  delay(ms);
}

void TrinketUtil_releaseKey() {
  // Unlike DigiKeyboard, it is necessary to release the keys.
  // Without releasing, the keys are permanently pressed.
  TrinketHidCombo.pressKey(0,0);
  TrinketHidCombo.poll();
}

static uint8_t gTrinketUtilRestoreCase;

void TrinketUtil_capsLockOff() {
  // With active CAPSLOCK there are errors with numbers and special characters.
  // CAPSLOCK is deactivated before PRINT if necessary.
  gTrinketUtilRestoreCase = 0;
  if ((TrinketHidCombo.getLEDstate() & KB_LED_CAPS) == KB_LED_CAPS) {
    gTrinketUtilRestoreCase = 1;
    TrinketHidCombo.pressKey(0,KEYCODE_CAPSLOCK); 
    TrinketUtil_releaseKey();
  }
}

void TrinketUtil_capsLockRestore() {
  // If CAPSLOCK was temporarily deactivated, it will be reactivated after the PRINT.
  // CAPSLOCK must not be changed on the computer during PRINT output.
  if (gTrinketUtilRestoreCase == 1) {
    if ((TrinketHidCombo.getLEDstate() & KB_LED_CAPS) != KB_LED_CAPS) {
      gTrinketUtilRestoreCase = 1;
      TrinketHidCombo.pressKey(0,KEYCODE_CAPSLOCK); 
      TrinketUtil_releaseKey();
    }
  }
}

#endif // IOTOOL_ATTINY85TRINKET_UTIL_H_
