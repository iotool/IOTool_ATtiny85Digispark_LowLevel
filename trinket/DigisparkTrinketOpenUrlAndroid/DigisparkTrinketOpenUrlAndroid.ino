// DigisparkTrinketOpenUrlAndroid.ino
// 
// Open www.google.com and create favorite (Linux, MacOS, Windows, Android)
// 
// 4468 Byte (68%) ROM / 94 Byte SRAM

#include <avr/io.h>
#define PINMODE_LED_INPUT        DDRB &= ~(1 << PB1)   // pinMode(1,INPUT)
#define PINMODE_LED_OUTPUT       DDRB |= (1 << PB1)    // pinMode(1,OUTPUT)
#define DIGITALWRITE_LED_LOW     PORTB &= ~(1 << PB1)  // digitalWrite(1,LOW)
#define DIGITALWRITE_LED_HIGH    PORTB |= (1 << PB1)   // digitalWrite(1,HIGH)

#include <util/delay.h>
#define DELAY_10_MS              _delay_ms(10)         // delay(10)

#include "TrinketHidCombo.h"

#define MODE_NORMAL              0  // plug into usb and don't press lock keys
#define MODE_CONFIG              1  // plug into usb and toggling lock keys
#define MODE_POWERBANK           2  // plug into powerbank or battery operated
static uint8_t gMode;

void Trinket_delay10ms(uint8_t loops);
void Trinket_typeStringProgMem(uint8_t index);
void Trinket_releaseKey();

void setup() {

  // init connection (1500ms) and timeout
  PINMODE_LED_INPUT;                          // use digispark buildin led
  DIGITALWRITE_LED_HIGH;                      // dimmed by internal pullup resistor
  TrinketHidCombo.begin();                    // start v-usb device
  uint8_t connectTimeout = 200;               // timeout 6000 millis (200x3x10ms)
  while(!TrinketHidCombo.isConnected()) {     // wait until connection is ready
    Trinket_delay10ms(3);                     // delay 20ms
    connectTimeout--;                         // reduce countdown
    if (connectTimeout == 0) {                // on timeout
      gMode = MODE_POWERBANK;                 // set mode powerbank
      return;                                 // exit setup
    }
  }

  // init keyboard (500ms)
  DIGITALWRITE_LED_LOW;                       // turn led off
  TrinketHidCombo.pressKey(0,0);              // init keyboard
  Trinket_delay10ms(50);                      // delay 500ms for stabilization

  // check lock keys (2000ms)
  DIGITALWRITE_LED_HIGH;                      // turn led on always dimmed
  uint8_t ledPrevState = TrinketHidCombo.getLEDstate() // init current led state
        , ledState                            // the led status depends on the keyboard
        , ledTimeout = 200                    // timeout 2000 millis (200x10ms)
        , ledChanges = 0;                     // number of keystrokes on the keyboard
  while (ledTimeout > 0) {                    // counting for 2 seconds
    ledTimeout--;                             // reduce countdown
    Trinket_delay10ms(1);                     // delay 10 millis
    ledState = TrinketHidCombo.getLEDstate(); // read current led state
    if (ledPrevState != ledState) {           // on key press
      ledPrevState = ledState;                // memorize led state
      ledChanges++;                           // increase keystroke counter
      if (ledChanges > 1) {                   // ignore read errors
        if (ledChanges & 0x00000001) {        // use counter for toggeling
          PINMODE_LED_OUTPUT;                 // turn led on
        } else {                              // and
          PINMODE_LED_INPUT;                  // dimm led
        }
      }
    }
  }

  // mode config or normal (default)
  if (ledChanges >= 4) {                      // more than random input of lock keys
    gMode = MODE_CONFIG;                      // set mode config
    PINMODE_LED_OUTPUT;                       // turn led on
    Trinket_typeStringProgMem(0);             // send output as keyboard
  } else {                                    // no or random input of the lock keys
    gMode = MODE_NORMAL;                      // set mode normal
    DIGITALWRITE_LED_LOW;                     // turn led off
    // Trinket_typeStringProgMem(7);          // send output as keyboard
  }

}

void loop() {
  Trinket_delay10ms(1);                       // keep alive v-usb connection
  switch(gMode) {                             // depending on the mode ..

    case MODE_NORMAL:                         // normal = led off

      // open browser (android)
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI,KEYCODE_B);
      Trinket_releaseKey();
      Trinket_delay10ms(100);

      // confirm single use
      TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
      Trinket_releaseKey();
      Trinket_delay10ms(50);

      // confirm first browser
      TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
      Trinket_releaseKey();
      Trinket_delay10ms(50);

      // open (linux)
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_ALT,KEYCODE_F2);
      Trinket_releaseKey();
      Trinket_delay10ms(150);

      // open (macos)
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI,KEYCODE_SPACE);
      Trinket_releaseKey();
      Trinket_delay10ms(150);

      // language (windows)
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI,KEYCODE_SPACE);
      Trinket_releaseKey();
      Trinket_delay10ms(50);
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI,KEYCODE_SPACE);
      Trinket_releaseKey();
      Trinket_delay10ms(50);
      
      // open (windows)
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_GUI,KEYCODE_R);
      Trinket_releaseKey();
      Trinket_delay10ms(150);
      
      // edit location url
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_CONTROL,KEYCODE_L);
      Trinket_releaseKey();
      Trinket_delay10ms(150);

      // http://google.com
      TrinketHidCombo.print(F("www.google.com"));
      Trinket_delay10ms(50);

      // confirm location
      TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
      Trinket_releaseKey();
      Trinket_delay10ms(50);

      // wait loading
      Trinket_delay10ms(1000);

      // add favorite
      TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_CONTROL,KEYCODE_D);
      Trinket_releaseKey();
      Trinket_delay10ms(150);

      // Google.com
      TrinketHidCombo.print(F("Google.com"));
      Trinket_delay10ms(100);

      // confirm favorite
      TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
      Trinket_releaseKey();
      Trinket_delay10ms(150);

      // confirm replace
      TrinketHidCombo.pressKey(0,KEYCODE_ENTER);
      Trinket_releaseKey();
      Trinket_delay10ms(50);

      // end of script
      while(1) {
        Trinket_delay10ms(1);
      }

      break;

    case MODE_CONFIG:                         // config = led on
      break;

    case MODE_POWERBANK:                      // powerbank = led dimmed
      // led min
      break;
  }
}

/* --- utilities --- */

void Trinket_delay10ms(uint8_t loops) {
  // poll() must be called at least once every 10ms to keep up 
  // the communication between microcontroller and computer via USB.
  while (loops > 0) { // loops > 0
    TrinketHidCombo.poll(); 
    loops--;
    DELAY_10_MS;
  }
}

const uint8_t gStringProgMem[] PROGMEM = {
  'C','o','n','f','i','g',0   // 0
 ,'g','o','o','g','l','e','.','c','o','m',0   // 7
};

void Trinket_typeStringProgMem(uint8_t index) {
  // Send predefined strings as keyboard via USB input device.
  // Save memory and read the strings directly from the binary.
  byte idx = index;
  uint8_t chr;
  do {
    chr = pgm_read_byte_near(gStringProgMem + idx);
    TrinketHidCombo.typeChar(chr);
    idx++;
  } while (chr > 0);
}

void Trinket_releaseKey() {
  // With the Trinket library the release of a key must be sent separately. 
  TrinketHidCombo.pressKey(0,0);
  TrinketHidCombo.poll();
}
