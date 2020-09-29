// DigisparkTrinketConfigSimpleUI.ino
// 
// Open editor like notepad, pluggin device into usb port and 
// toogle NUM lock key for 10 seconds until display starts.
// 
//   C 0..3     command (E)dit (L)oad (S)ave (C)lear
//   A 0..255   address
//   V 0..255   value
// 
//   NUM down  |  SCROLL up  |  CAPS next row
// 
// Choise field (command | address | value) by press CAPS lock key.
// Increase value of field by NUM lock key and decrease by SCROLL lock key.
//
// load value:
// - command edit, choise address, goto value
// - command load, goto address (load), goto value
// 
// save value:
// - command edit, choise address, choise value
// - command save, goto address (save), goto value
// 
// 5154 Byte (79%) ROM / 104 Byte (21%) SRAM

#include <avr/io.h>
#define PINMODE_LED_INPUT        DDRB &= ~(1 << PB1)   // pinMode(1,INPUT)
#define PINMODE_LED_OUTPUT       DDRB |= (1 << PB1)    // pinMode(1,OUTPUT)
#define DIGITALWRITE_LED_LOW     PORTB &= ~(1 << PB1)  // digitalWrite(1,LOW)
#define DIGITALWRITE_LED_HIGH    PORTB |= (1 << PB1)   // digitalWrite(1,HIGH)

#include <util/delay.h>
#define DELAY_10_MS              _delay_ms(10)         // delay(10)

#include <avr/eeprom.h>

#include "TrinketHidCombo.h"
#define KEYCODE_CAPSLOCK         0x39 // corresponding status led KB_LED_CAPS
#define KEYCODE_SCROLLLOCK       0x47 // corresponding status led KB_LED_SCROLL
#define KEYCODE_NUMLOCK          0x53 // corresponding status led KB_LED_NUM

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
    Trinket_delay10ms(250);                   // delay 2500ms before start
    // Trinket_typeStringProgMem(0);          // send output as keyboard
  } else {                                    // no or random input of the lock keys
    gMode = MODE_NORMAL;                      // set mode normal
    DIGITALWRITE_LED_LOW;                     // turn led off
  }

}

void loopConfig();

void loop() {
  Trinket_delay10ms(1);                       // keep alive v-usb connection
  
  switch(gMode) {                             // depending on the mode ..

    case MODE_NORMAL:                         // normal = led off
      break;

    case MODE_CONFIG:                         // config = led on
      loopConfig();
      break;

    case MODE_POWERBANK:                      // powerbank = led dimmed
      // led min
      break;
  }
}

static uint8_t gLedPrevState = 16;
static uint8_t gConfigInterface[] = {         // interface
  1,                                          // position
  0,                                          // command
  0,                                          // address
  0                                           // value
};
static uint8_t gLedStateCode[] = {
  KB_LED_NUM,
  KB_LED_SCROLL,
  KB_LED_CAPS
};
static uint8_t gLedKeyCode[] = {
  KEYCODE_NUMLOCK,
  KEYCODE_SCROLLLOCK,
  KEYCODE_CAPSLOCK
};

void loopConfig() {

  // read the input from the keyboard via the corresponding led status
  uint8_t ledState = 0b00000111 & TrinketHidCombo.getLEDstate();

  // send keystroke to turn off led 
  for (uint8_t i=0;i<=2;i++) {
    if ((ledState & gLedStateCode[i]) == gLedStateCode[i]) {
      TrinketHidCombo.pressKey(0,gLedKeyCode[i]);
      Trinket_releaseKey();
      Trinket_delay10ms(10);
    }
  }

  // if state has changed
  if ((gLedPrevState != ledState) & (ledState == 0)) {

    // update interface
    switch(gLedPrevState) {
      case KB_LED_NUM:
        gConfigInterface[gConfigInterface[0]]--;
        break;
      case KB_LED_SCROLL:
        gConfigInterface[gConfigInterface[0]]++;
        break;
      case KB_LED_CAPS:
        gConfigInterface[0]++;
        if (gConfigInterface[0]>3) {
          gConfigInterface[0]=1;
        }
        break;
    }

    // command overflow
    if (gConfigInterface[1]>3) {
      gConfigInterface[1] = 0;
    }

    // clear screen = select all for deletion
    TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_CONTROL,KEYCODE_A);
    Trinket_releaseKey();

    // display interface as ascii digits
    for (uint8_t i=1;i<=3;i++) {
      if (i != gConfigInterface[0]) {
        Trinket_typeStringProgMem(0);
      } else {
        if (i==1) {
          Trinket_typeStringProgMem(22+3*gConfigInterface[1]);
        } else {
          Trinket_typeStringProgMem(3*i);
        }
      }
      TrinketHidCombo.println((uint16_t)gConfigInterface[i]);
    }

    // handle eeprom
    if ((gConfigInterface[0]==2) & (gConfigInterface[1]>0)) {
      uint16_t eepromAdr = gConfigInterface[2];
      uint8_t  eepromVal = gConfigInterface[3];
      switch(gConfigInterface[1]) {
        case 1: // load
          eeprom_read_block((void*)&eepromVal,(const void*)eepromAdr,1);
          gConfigInterface[3] = eepromVal;
          Trinket_typeStringProgMem(12);
          break;
        case 2: // save
          eeprom_write_block((const void*)&eepromVal,(void*)eepromAdr,1);
          Trinket_typeStringProgMem(17);
          break;
        case 3: // clear
          gConfigInterface[3] = 0;
          break;
      }
      gConfigInterface[1]=0;
    }

  }

  gLedPrevState = ledState;

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
  ' ',' ',0            // 0
 ,'C',' ',0            // 3
 ,'A',' ',0            // 6
 ,'V',' ',0            // 9
 ,'l','o','a','d',0    // 12
 ,'s','a','v','e',0    // 17
 ,'E',' ',0            // 22 EDIT
 ,'L',' ',0            // 25 LOAD
 ,'S',' ',0            // 28 SAVE
 ,'C',' ',0            // 31 CLEAR
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
