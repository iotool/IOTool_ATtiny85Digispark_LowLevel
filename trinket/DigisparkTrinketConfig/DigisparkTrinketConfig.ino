// DigisparkTrinketConfig.ino
// 
// User interface for reading and writing the EEPROM.
// An input field or editor is used for display a dialog.
// The lock keys NUM and SCROLL are used for navigation.
// 
//   .----------- index   [1..7]
//   |.---------- command [0..9]
//   ||  .------- adress  [0..511|0..63]
//   ||  |  .---- value
//   ||  |  |
//   icaaavvv     SCROLL next index | NUM next value
//   10000000
// 
//   Examples:
//   12001000     READCELL Address 001 .. confirm with SCROLL
//   22001000123  READCELL Address 001 Value 123 .. wait until NUM
//   14001012     WRITE    Address 001 Value 012 .. confirm with SCROLL
//   15001012     WAITLOCK .. wait until NUM
// 
// To start in config dialog open texteditor like notepad, press and release a SCROLL LOCK key
// on your keyboard for about 10 seconds after plugging the Digispark into the USB port.
// After about 5 seconds the LED on the Digispark will flash according to your keystrokes.
// 
// Your editor is used to display the dialog. No additional software needs to be installed. 
// Using the LOCK keys NUM and SCROLL the configuration in the EEPROM can be changed.
// There are three methods: read data, write data and interrupt output.
// 
// 5040 Byte (77%) ROM / 103 Byte (21%) SRAM

// comment for disable the feature
// #define CONFIG_USE_READ       // 1: +180 bytecode  parse value for interface
#define CONFIG_USE_READCELL      // 2: +110 bytecode  print value after interface
// #define CONFIG_USE_READBLOCK  // 3: +188 bytecode  println block of 8 values
#define CONFIG_USE_WRITE         // 4:  +72 bytecode  write value
#define CONFIG_USE_WAITLOCK      // 5:  +30 bytecode  wait until numlock pressed
// #define CONFIG_USE_LOWFREQ    //      +4 bytecode  slow refresh frequency

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

static uint8_t gLedState;
static uint8_t gConfigInterface[] = {         // interface
  1,                                          // position
  0,                                          // command
  0,0,0,                                      // address
  0,0,0                                       // value
};

void loopConfig() {

  // read the input from the keyboard via the corresponding led status
  gLedState = TrinketHidCombo.getLEDstate();

  // scroll = next interface position
  if ((gLedState & KB_LED_SCROLL) == KB_LED_SCROLL) {
    // confirm input and switch off the led by pressing the lock key
    TrinketHidCombo.pressKey(0,KEYCODE_SCROLLLOCK);
    Trinket_releaseKey();
    // switch to the next position in the interface
    gConfigInterface[0]++;
    if (gConfigInterface[0] > 7) {
      gConfigInterface[0] = 1;
    }
  }

  // num = next value of the digit
  if ((gLedState & KB_LED_NUM) == KB_LED_NUM) {
    // confirm input and switch off the led by pressing the lock key
    TrinketHidCombo.pressKey(0,KEYCODE_NUMLOCK);
    Trinket_releaseKey();
    // switch to the next value of the digit
    gConfigInterface[gConfigInterface[0]]++;
    if (gConfigInterface[gConfigInterface[0]]>9) {
      gConfigInterface[gConfigInterface[0]]=0;
    }
  }

  // clear screen = select all for deletion
  TrinketHidCombo.pressKey(KEYCODE_MOD_LEFT_CONTROL,KEYCODE_A);
  Trinket_releaseKey();

  // display interface as ascii digits
  for (uint8_t i=0;i<=7;i++) {
    TrinketHidCombo.print((char)(0x30+gConfigInterface[i]));
  }

  // execute command, if requested
  if ((gConfigInterface[0]==2) & (gConfigInterface[1]>0)) {
    uint16_t eepromAdr=gConfigInterface[2]*100+gConfigInterface[3]*10+gConfigInterface[4];
    uint8_t eepromVal=gConfigInterface[5]*100+gConfigInterface[6]*10+gConfigInterface[7];

    // reset position
    gConfigInterface[0] = 1;

#ifdef CONFIG_USE_READ
    // load value from eeprom address
    if (gConfigInterface[1] == 1) {
      eeprom_read_block((void*)&eepromVal,(const void*)eepromAdr,1);
      for (uint8_t x=0;x<9;x++) {
        for (uint8_t y=0;y<9;y++) {
          for (uint8_t z=0;z<9;z++) {
            if (eepromVal == (100*x+10*y+z)) {
              gConfigInterface[5]=x;
              gConfigInterface[6]=y;
              gConfigInterface[7]=z;
            }
          }
        }
      }
    }
#endif // CONFIG_USE_READ

#ifdef CONFIG_USE_READCELL
    // load value from eeprom address
    if (gConfigInterface[1] == 2) {
      eeprom_read_block((void*)&eepromVal,(const void*)eepromAdr,1);
      TrinketHidCombo.print((uint16_t)eepromVal);
      gConfigInterface[1] = 5;
    }
#endif // CONFIG_USE_READCELL

#ifdef CONFIG_USE_READBLOCK
    // save value to eeprom address
    if (gConfigInterface[1] == 3) {
      uint8_t eepromBlock[8];
      eepromAdr*=8;
      eeprom_read_block((void*)&eepromBlock,(const void*)eepromAdr,8);
      TrinketHidCombo.println();
      for (uint8_t i=0;i<=7;i++) {
        TrinketHidCombo.println((uint16_t)eepromBlock[i]);
      }
      gConfigInterface[1] = 5;
    }
#endif // CONFIG_USE_READBLOCK

#ifdef CONFIG_USE_WRITE
    // save value to eeprom address
    if (gConfigInterface[1] == 4) {
      eeprom_write_block((const void*)&eepromVal,(void*)eepromAdr,1);
    }
#endif // CONFIG_USE_WRITE

#ifdef CONFIG_USE_WAITLOCK
    // wait until scroll lock key is pressed
    if (gConfigInterface[1] == 5) {
      gConfigInterface[0] = 0;
      while (!((TrinketHidCombo.getLEDstate() & KB_LED_NUM) == KB_LED_NUM)) {
        Trinket_delay10ms(1);
      }
    }
#endif // CONFIG_USE_WAITLOCK

    // wait for the next command
    gConfigInterface[1] = 0;  // clear command
    Trinket_delay10ms(10);    // more time for reader
  }

#ifdef CONFIG_USE_LOWFREQ
  // reduce display frequency
  Trinket_delay10ms(50);
#endif // CONFIG_USE_LOWFREQ

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

void Trinket_releaseKey() {
  // With the Trinket library the release of a key must be sent separately. 
  TrinketHidCombo.pressKey(0,0);
  TrinketHidCombo.poll();
}
