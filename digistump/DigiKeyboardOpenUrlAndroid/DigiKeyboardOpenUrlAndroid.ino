// DigiKeyboardOpenUrlAndroid.ino
//
// Open url www.google.com and add to favorites (Linux, MacOS, Windows, Android)
//
// 3444 Byte (52%) ROM / 81 Byte SRAM

#include "DigiKeyboard.h"

void setup() {
  // wait until connected
  pinMode(1,INPUT_PULLUP);
  DigiKeyboard.sendKeyStroke(0);
  delay(2000);
  // ready to use
  pinMode(1,OUTPUT);
  digitalWrite(1,HIGH);
}


void loop() {

  // open browser (android)
  DigiKeyboard.sendKeyStroke(KEY_B,MOD_GUI_LEFT);
  DigiKeyboard.delay(1000);

  // confirm single use
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);

  // confirm first browser
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);

  // open (linux)
  DigiKeyboard.sendKeyStroke(KEY_F2,MOD_ALT_LEFT);
  DigiKeyboard.delay(1500);

  // open (macos)
  DigiKeyboard.sendKeyStroke(KEY_SPACE,MOD_GUI_LEFT);
  DigiKeyboard.delay(1500);

  // language (windows)
  DigiKeyboard.sendKeyStroke(KEY_SPACE,MOD_GUI_LEFT);
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_SPACE,MOD_GUI_LEFT);
  DigiKeyboard.delay(500);
  
  // open (windows)
  DigiKeyboard.sendKeyStroke(KEY_R,MOD_GUI_LEFT);
  DigiKeyboard.delay(1500);
  
  // edit location url (Android)
  DigiKeyboard.sendKeyStroke(KEY_L,MOD_CONTROL_LEFT);
  DigiKeyboard.delay(1500);

  // http://google.com
  DigiKeyboard.print(F("www.google.com"));
  DigiKeyboard.delay(500);

  // confirm location
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);

  // wait loading
  DigiKeyboard.delay(10000);

  // add favorite
  DigiKeyboard.sendKeyStroke(KEY_D,MOD_CONTROL_LEFT);
  DigiKeyboard.delay(1500);

  // Google.com
  DigiKeyboard.print(F("Google.com"));
  DigiKeyboard.delay(1000);

  // confirm favorite
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(1500);

  // confirm replace
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboard.delay(500);

  // end of script
  digitalWrite(1,LOW);
  while(1) {
    DigiKeyboard.delay(10);
  }

}
