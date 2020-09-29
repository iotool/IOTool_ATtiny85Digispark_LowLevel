// DigisparkTrinketBootModes.ino
// 
// Enter config mode and detect powerbank or non usb slot.
// Support three different modes:
// 
//   MODE_NORMAL       plug into usb and don't press lock keys
//   MODE_CONFIG       plug into usb and toggling lock keys
//   MODE_POWERBANK    plug into powerbank or battery operated
// 
// This is a proof of concept to use TrinketHidCombo library 
// to differentiate between usb port and powerbank and 
// the use of the three keyboard status leds and lock keys as input.
// 
// To start in config mode, press and release a LOCK key (CAPS, NUM, SCROLL) 
// on your keyboard for about 10 seconds after plugging the Digispark into the USB port.
// After about 5 seconds the LED on the Digispark will flash according to your keystrokes.
// 
// 4072 Byte (62%) ROM / 93 Byte (19%) SRAM

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

void setup() {

  // init connection (1500ms) and timeout
  PINMODE_LED_INPUT;                          // use digispark buildin led
  DIGITALWRITE_LED_HIGH;                      // dimmed by internal pullup resistor
  TrinketHidCombo.begin();                    // start v-usb device
  uint8_t connectTimeout = 200;               // timeout 4000 millis (200x2x10ms)
  while(!TrinketHidCombo.isConnected()) {     // wait until connection is ready
    Trinket_delay10ms(2);                     // delay 20ms
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
  }

}

void loop() {
  Trinket_delay10ms(1);                       // keep alive v-usb connection
  switch(gMode) {                             // depending on the mode ..

    case MODE_NORMAL:                         // normal = led off
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
  'C','o','n','f','i','g',0
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
