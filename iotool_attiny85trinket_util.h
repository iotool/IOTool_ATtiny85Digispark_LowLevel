// iotool_attiny85trinket_util.h
// 
// Utility for library TrinketHidCombo.h as replacement 
// for DigiKeyboard and DigiMouse.
// 
// https://github.com/iotool/IOTool_ATtiny85Digispark_LowLevel
// 
// 
// 2020-09-25  RoHa  v1.0 delay

#ifndef IOTOOL_ATTINY85TRINKET_UTIL_H_
#define IOTOOL_ATTINY85TRINKET_UTIL_H_

void delayTrinket(uint16_t ms) {
  // poll() must be called at least once every 10ms
  while (ms>10) {
    TrinketHidCombo.poll(); 
    ms-=10;
    delay(10);
  }
  TrinketHidCombo.poll(); 
  delay(ms);
}

#endif // IOTOOL_ATTINY85TRINKET_UTIL_H_
