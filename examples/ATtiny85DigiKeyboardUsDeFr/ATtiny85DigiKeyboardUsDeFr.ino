// ATtiny85DigiKeyboardUsDeFr.ino

#include "DigiKeyboard.h"      // qwerty
#include "DigiKeyboardDe.h"    // qwertz
#include "DigiKeyboardFr.h"    // awerty
#include "DigiKeyboardUtil.h"  // util

#define BUILDIN_LED 1

void setup() {
  DigiKeyboard.sendKeyStroke(0,0);
  DigiKeyboard.delay(4000);
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT);
  DigiKeyboard.delay(500);
  DigiKeyboard_print("notepad");
  DigiKeyboard.sendKeyStroke(KEY_ENTER,0);
  DigiKeyboard.delay(500);
}

void loop() {
  // US 1234567890 !"$%&/()=?\+*~#',.-;:_@<|>
  // DE 1234567890 !"$%&/()=?\+*~#',.-;:_@<|>
  // FR 1234567890 !"$%&/()=?\+*~#',.-;:_@<|>
  String lText = "1234567890 !\"$%&/()=?\\+*~#',.-;:_@<|>";
  DigiKeyboard.delay(10);
  DigiKeyboard_setKeyboardLayout(KEYBOARD_LAYOUT_US);
  DigiKeyboard_print("US "); 
  DigiKeyboard_print(lText);
  DigiKeyboard.sendKeyStroke(KEY_ENTER,0);
  DigiKeyboard.delay(500);
  DigiKeyboard_setKeyboardLayout(KEYBOARD_LAYOUT_DE);
  DigiKeyboard_print("DE "); 
  DigiKeyboard_print(lText);
  DigiKeyboard.sendKeyStroke(KEY_ENTER,0);
  DigiKeyboard.delay(500);
  DigiKeyboard_setKeyboardLayout(KEYBOARD_LAYOUT_FR);
  DigiKeyboard_print("FR ");
  DigiKeyboard_print(lText);
  DigiKeyboard.sendKeyStroke(KEY_ENTER,0);
  DigiKeyboard.delay(500);
  DigiKeyboard.delay(5000);
}
