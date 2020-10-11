// DigiStepper28BYJ48.ino
// 
// PB0 = IN4
// PB1 = IN3
// PB2 = IN2
// PB3 = IN1
// GND = 5-
// 5V  = 5+
// 
// USB-Port
// 15 u/min (12x 629us delay = 7,548ms delay/loop)
// Gewinde M6 = 1,00mm/u = 10u/cm (40s/cm)
// Gewinde M7 = 1,00mm/u = 10u/cm (40s/cm)
// Gewinde M8 = 1,25mm/u =  8u/cm (32s/cm)
// 
// Powerbank
// 12 u/min (12x 792us delay = 9,504ms delay/loop)
// Gewinde M6 = 1,00mm/u = 10u/cm (50s/cm)
// Gewinde M7 = 1,00mm/u = 10u/cm (50s/cm)
// Gewinde M8 = 1,25mm/u =  8u/cm (40s/cm)

#include <util/delay.h>

#define STEPPER_COIL_1 0b00001000
#define STEPPER_COIL_2 0b00000100
#define STEPPER_COIL_3 0b00000010
#define STEPPER_COIL_4 0b00000001

void stepperMove(uint8_t pCoil) {
  if ((pCoil & STEPPER_COIL_1) == STEPPER_COIL_1) {
    pinMode(0,OUTPUT);
    digitalWrite(0,HIGH);
  } else {
    pinMode(0,OUTPUT);
    digitalWrite(0,LOW);
  }
  if ((pCoil & STEPPER_COIL_2) == STEPPER_COIL_2) {
    pinMode(1,OUTPUT);
    digitalWrite(1,HIGH);
  } else {
    pinMode(1,OUTPUT);
    digitalWrite(1,LOW);
  }
  if ((pCoil & STEPPER_COIL_3) == STEPPER_COIL_3) {
    pinMode(2,OUTPUT);
    digitalWrite(2,HIGH);
  } else {
    pinMode(2,OUTPUT);
    digitalWrite(2,LOW);
  }
  if ((pCoil & STEPPER_COIL_4) == STEPPER_COIL_4) {
    pinMode(3,OUTPUT);
    digitalWrite(3,HIGH);
  } else {
    pinMode(3,OUTPUT);
    digitalWrite(3,LOW);
  }
}

void stepperWait(uint8_t pWait) {
  // 12x loop = 15 u/um
  uint8_t lDelay = pWait;
  while(lDelay>=1) {
    lDelay--;
    _delay_us(629);
  }
}

void setup() {
  stepperMove(0b00000000);
}

void loop() {
  stepperMove(0b00001100); stepperWait(1);
  stepperMove(0b00000100); stepperWait(2);
  stepperMove(0b00000110); stepperWait(1);
  stepperMove(0b00000010); stepperWait(2);
  stepperMove(0b00000011); stepperWait(1);
  stepperMove(0b00000001); stepperWait(2);
  stepperMove(0b00001001); stepperWait(1);
  stepperMove(0b00001000); stepperWait(2);
}
