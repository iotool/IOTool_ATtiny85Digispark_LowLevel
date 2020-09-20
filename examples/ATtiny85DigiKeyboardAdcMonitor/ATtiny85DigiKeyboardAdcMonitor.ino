// ATtiny85DigiKeyboardAdcMonitor.ino
// 
// Log analog values of PB0,PB1,PB2 and PB5 to csv or excel.
// tbd: http://21stdigitalhome.blogspot.com/2014/10/trinket-attiny85-internal-temperature.html
// 
// PB0 digitalRead
// PB1 digitalRead
// PB2 analogRead
// PB5 analogRead
// GAP floating by buildin led and pb5/gnd or pb5/pb4/pb3 touch
// BTN (N)one (D)ark (T)ouch (U)ndefined

#include "DigiKeyboard.h"
// https://cdn.hackaday.io/files/10446446921664/DigiKeyboard_6keys.h
#define KEY_TAB               0x2B     // Keyboard Tab

void setup() {
  pinMode(0,INPUT);
  pinMode(1,INPUT);
  pinMode(2,INPUT);
  pinMode(5,INPUT);
}

static byte state = 0;

void DigiKeyboardNext() {
  DigiKeyboard.delay(100);
}

void loop() {
  if (state == 0) {
    state = 1;
    /*--- init keyboard ---*/
    DigiKeyboard.sendKeyStroke(0);
    /*--- blink buildin gap ---*/
    pinMode(1,OUTPUT); 
    digitalWrite(1,HIGH);
    DigiKeyboard.delay(3000);
    digitalWrite(1,LOW);
    pinMode(1,INPUT); 
    DigiKeyboard.delay(1000);
    /*--- start notepad by keyboard ---*/
    DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // Windows+R
    DigiKeyboardNext();
    DigiKeyboard.print("notepad");
    DigiKeyboardNext();
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboardNext();
    DigiKeyboard.delay(3000);
    /*--- header ---*/
    DigiKeyboard.print(F("PB0"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("PB1"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("PB2"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("PB5"));
    // DigiKeyboard.sendKeyStroke(KEY_TAB);
    // DigiKeyboard.print(F("TMP"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("GAP"));
    DigiKeyboard.sendKeyStroke(KEY_TAB);
    DigiKeyboard.print(F("BTN"));
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboardNext();
  }
  /*--- read values ---*/
  uint16_t pb0,pb1,pb2,pb5,tmp,gap;
  pb0 = digitalRead(0);
  pb1 = digitalRead(1);
  pb2 = analogRead(1); // PB2 = ADC1
  pb5 = analogRead(0); // PB5 = ADC0
  // tmp = analogRead(4); // TMP = ADC4
  /*--- read floating ---*/
  uint16_t adcBuffer[4] = {0};
  gap = 0;
  for (byte k=0; k<16; k++) {
    // Use buildin led for floating between LO and HI
    // The measured value of adc0 is influenced by the fluctuation.
    // Covering the built-in LED increases the fluctuation.
    // Touching PB5 and GND increases the fluctuation very much (Lilytiny).
    // Touch is also detected for pins PB5 and PB4 / PB3.
    adcBuffer[2] = adcBuffer[0];
    adcBuffer[3] = adcBuffer[1];
    // set PB1 to ground
    pinMode(1,OUTPUT);    
    adcBuffer[0] = analogRead(0); // PB5 = ADC0
    DigiKeyboard.delay(1);
    // set PB1 floating
    pinMode(1,INPUT);    
    // adcBuffer[1] = analogRead(1); // PB2 = ADC1
    adcBuffer[1] = analogRead(0); // PB5 = ADC0
    DigiKeyboard.delay(1);
    // add up all fluctuations
    if (k>0) {
      for (byte i=1; i<4; i++) {
        if (adcBuffer[i-1]<adcBuffer[i]) {
          gap += adcBuffer[i]-adcBuffer[i-1];
        } else {
          gap += adcBuffer[i-1]-adcBuffer[i];
        }
      }
    }
  }
  /*--- display values ---*/
  DigiKeyboard.print(pb0);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  DigiKeyboard.print(pb1);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  DigiKeyboard.print(pb2);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  DigiKeyboard.print(pb5);
  // DigiKeyboard.sendKeyStroke(KEY_TAB);
  // DigiKeyboard.print(tmp);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  DigiKeyboard.print(gap);
  DigiKeyboard.sendKeyStroke(KEY_TAB);
  if ((gap>=0) && (gap<=200)) {
    // normal fluctuation
    DigiKeyboard.print(F("N"));
  } else if ((gap>=220) && (gap<=400)) {
    // darkness - covered LED
    DigiKeyboard.print(F("D"));
  } else if ((gap>=800) && (gap<=4000)) {
    // touch - touch PB5 and GND with finger
    DigiKeyboard.print(F("T"));
  } else {
    // undefined - no clear assignment
    DigiKeyboard.print(F("U"));
  }
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  // DigiKeyboardNext();
  if ((gap>=800) && (gap<=4000)) {
    // Built-in LED flashes weakly when touch sensor is detected.
    // Uses the internal pullup resistor of 25k ohm.
    pinMode(1,INPUT_PULLUP);    
    DigiKeyboard.delay(70);
    pinMode(1,OUTPUT);    
    digitalWrite(1,LOW);
    pinMode(1,INPUT);    
    DigiKeyboard.delay(14);
  } else {
    DigiKeyboard.delay(84);
  }
}
