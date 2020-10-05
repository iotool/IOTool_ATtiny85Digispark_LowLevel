// DigiKeyboardPowerbankKeepAlive.ino
// 
// Detect power source by zenner diode.
// If detect powerbank produce > 100mA current for 100ms every 10s.
// To keep the powerbank alive you have to draw more then 100mA current.
// Most powerbank turn off after 15 seconds if there is no high current draw.
// We use a 47 ohm resistor to draw 90mA for 100ms.
//
//                |+5V           current peek
//               [47]  89mA      99mA = 89mA + 10mA
//                |
// PB1 --[470]--|<  N2222        1% = 100ms/10.000ms
//        10mA    |-GND          1mA extra current draw
//
// 
// Add isReady() to DigiKeyboard.h to define v-usb timeout.
// Execute error() if device connect to powerbank or battery.
// Reduce current to 10 mA by reducing the clock speed durring delay.
// 
// 20.000mAh QC3 : 375ms@23R + delay(17ms+96ms+512ms)
//  2.400mAh     : 100ms@47R + delay(900ms+9*1000ms)

#include "DigiKeyboard.h"

#define POWER_USB_COMPUTER  1  // led off
#define POWER_USB_POWERBANK 2  // led slow blink
#define POWER_BAT_4V5       3  // led fast tick 1x
#define POWER_BAT_9V0       4  // led fast tick 2x
// #define POWER_DEV_20KQC3

#define CHANGE_CPU_1MHZ  cli();CLKPR=(1<<CLKPCE);CLKPR=0;sei();
#define CHANGE_CPU_31KHZ cli();CLKPR=(1<<CLKPCE);CLKPR=5;sei();
#define CHANGE_CPU_4KHZ  cli();CLKPR=(1<<CLKPCE);CLKPR=8;sei();

static uint8_t gPowerSource;

void setup() {

  // dim led by internal resistor
  pinMode(1,INPUT_PULLUP);
    
  // wait 5 seconds to connect
  uint16_t initTimeout = 5000;
  while (!DigiKeyboard.isReady()) {
    initTimeout -= 5; // delay of isReady
    if (initTimeout == 0) {
      pinMode(3,INPUT);
      initTimeout += analogRead(3);
      initTimeout += analogRead(3);
      initTimeout += analogRead(3);
      initTimeout += analogRead(3);
      initTimeout = initTimeout >> 2;
      if (initTimeout > 636) {
        // ADC=660 (PB3~2.93V 5V~4.54V)        
        gPowerSource = POWER_BAT_4V5;
      }      
      else if (initTimeout > 601) {
        // ADC=613 (PB3~3.01V 5V~5.02V)
        gPowerSource = POWER_BAT_9V0;
      }
      else { // initTimeout <= 601
        // ADC=591 (PB3~2.78V 5V~4.81V)
        gPowerSource = POWER_USB_POWERBANK;
      }
      pinMode(0,INPUT);
      pinMode(2,INPUT);
      pinMode(3,INPUT);
      pinMode(4,OUTPUT);
      digitalWrite(4,LOW);
      pinMode(5,INPUT);
      return;
    }
  }
  gPowerSource = POWER_USB_COMPUTER;
    
  // initial keystroke
  DigiKeyboard.sendKeyStroke(0);
  delay(500);
  
  // ready to use
  pinMode(1,OUTPUT);
  digitalWrite(1,HIGH);

}

static int16_t gBatteryTimeout=999999;

void battery() {
    
  // toggle led to signal battery
  switch(gPowerSource) {
    case POWER_USB_POWERBANK: // 10.8mA = 140mA(1%) + 9.5mA(99%)
      // keep alive powerbank
#ifdef POWER_DEV_20KQC3
        CHANGE_CPU_1MHZ;
        gBatteryTimeout = 0;
        pinMode(1,OUTPUT); 
        digitalWrite(1,HIGH); // HI: 3mA@1k 93mA@470/47+47
        pinMode(3,OUTPUT);
        digitalWrite(3,HIGH); // HI: 9.9mA@1k5/220 29.84mA@1k/68
        pinMode(4,OUTPUT); 
        digitalWrite(4,HIGH); // HI: 8.6mA@220 27.94mA@68est
        delay(375); gBatteryTimeout += 375;
        digitalWrite(1,LOW);
        pinMode(3,INPUT);
        digitalWrite(4,LOW);
        delay(17); gBatteryTimeout += 17;
        CHANGE_CPU_31KHZ;
        delay(3); gBatteryTimeout += 96;
        CHANGE_CPU_4KHZ;
        delay(2); gBatteryTimeout += 512;
#else // not POWER_DEV_20KQC3
      if (gBatteryTimeout >= 10000) {
        CHANGE_CPU_1MHZ;
        gBatteryTimeout = 0;
        pinMode(1,OUTPUT); 
        digitalWrite(1,HIGH); // HI: 3mA@1k 93mA@470/47
        pinMode(3,OUTPUT);
        digitalWrite(3,HIGH); // HI: 9.9mA@1k5/220 29.84mA@1k/68
        pinMode(4,OUTPUT); 
        digitalWrite(4,HIGH); // HI: 8.6mA@220 27.94mA@68est
        delay(100); gBatteryTimeout += 100;
        digitalWrite(1,LOW);
        pinMode(3,INPUT);
        digitalWrite(4,LOW);
        delay(4); gBatteryTimeout += 4;
        CHANGE_CPU_31KHZ;
        delay(4); gBatteryTimeout += 128;
        CHANGE_CPU_4KHZ;
        delay(3); gBatteryTimeout += 768;
      } else {
        // blink
        CHANGE_CPU_1MHZ;
        pinMode(1,OUTPUT);
        digitalWrite(1,HIGH);
        delay(8); gBatteryTimeout += 8;
        digitalWrite(1,LOW);
        CHANGE_CPU_31KHZ;
        delay(7); gBatteryTimeout += 224;
        CHANGE_CPU_4KHZ;
        delay(3); gBatteryTimeout += 768;
      }
#endif // not POWER_DEV_20KQC3
      break;
    case POWER_BAT_9V0: // 10.4mA = 22.5mA(7%) + 9.5mA(93%)
      CHANGE_CPU_1MHZ;
      pinMode(1,INPUT_PULLUP);
      delay(72); // =72ms 1*72
      pinMode(1,OUTPUT);
      digitalWrite(1,LOW);
      CHANGE_CPU_31KHZ;
      delay(3); // ~96ms 32*3
      pinMode(1,INPUT_PULLUP);
      delay(2); // ~64ms 32*2
      pinMode(1,OUTPUT);
      digitalWrite(1,LOW);
      CHANGE_CPU_4KHZ;
      delay(3); // ~768ms
      break;
    case POWER_BAT_4V5: // 9.0mA = 22.5mA(7%) + 8.0mA(93%)
     CHANGE_CPU_1MHZ;
      pinMode(1,INPUT_PULLUP);
      delay(72); // =72ms 1*72
      pinMode(1,OUTPUT);
      digitalWrite(1,LOW);
      CHANGE_CPU_31KHZ;
      delay(5); // ~160ms 32*5
      CHANGE_CPU_4KHZ;
      delay(3); // ~768ms 256*3
      break;
  }

    
}

void loop() {

  // on error
  if (gPowerSource != POWER_USB_COMPUTER) {
    battery();
    return;
  }
  
  // send keystrokes
  DigiKeyboard.print(F("test"));

  // end of script
  digitalWrite(1,LOW);
  while(1) {
    DigiKeyboard.delay(10);
  }

}
