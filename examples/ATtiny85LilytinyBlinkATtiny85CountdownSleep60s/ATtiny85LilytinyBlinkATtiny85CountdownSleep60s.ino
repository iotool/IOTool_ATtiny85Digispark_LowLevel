// Countdown 60 seconds for ATtiny85 Lilytiny
// 
// Countdown every 5 seconds and tick buildin led at pb1.
// Turn on pb4 und pb1 at the end of countdown.
// Current usage 6.9mA*4000ms + 11.2mA*900ms + 18mA*100ms = 7.9mA
// 
// 384 Byte (6% of 6.012) / RAM 2 Byte

#include "iotool_attiny85lilytiny_lowlevel.h"

ISR (PCINT0_vect) {
}

ISR (WDT_vect) {
}

void resetWatchdog () {
  // clear various "reset" flags
  MCUSR = 0;    
  // allow changes, disable reset, clear existing interrupt
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)
  // WDTCR = bit (WDIE) | bit (WDP2) | bit (WDP1);    // set WDIE, and 1 seconds delay
  WDTCR = bit (WDIE) | bit (WDP2) | bit (WDP1) | bit (WDP0); // set WDIE, and 2 seconds delay
  // WDTCR = bit (WDIE) | bit (WDP3);    // set WDIE, and 4 seconds delay
  // WDTCR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  // pat the dog
  wdt_reset();  
}  // end of resetWatchdog

void sleepPowerDown() {
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  noInterrupts ();       // timed sequence coming up
  resetWatchdog ();      // get watchdog ready
  sleep_enable ();       // ready to sleep
  interrupts ();         // interrupts are required now
  sleep_cpu ();          // sleep                
  sleep_disable ();      // precaution
  power_all_enable ();   // power everything back on
}

void sleepWakeup() {
  wdt_disable();
}

static uint16_t countdown = 60;

int main() 
{
  // --- setup ---
  PINMODE_PB0_OUTPUT;
  PINMODE_PB1_OUTPUT;
  PINMODE_PB2_OUTPUT;
  PINMODE_PB3_INPUT;
  PINMODE_PB4_OUTPUT;
  PINMODE_PB5_INPUT;
  DIGITALWRITE_PB0_LOW;
  DIGITALWRITE_PB1_LOW;
  DIGITALWRITE_PB2_LOW;
  DIGITALWRITE_PB4_LOW;
  DIGITALWRITE_PB5_HIGH;
  CHANGE_CPU_1MHZ;     // 17.00mA
  power_adc_disable(); // 16.60mA
  power_all_disable(); // 14.40mA
  resetWatchdog();

  // --- loop ---
  while(1)                // forever
  {
    loop();            
  }
}

void loop() 
{
  if (countdown == 0) {
    DIGITALWRITE_PB1_HIGH;
    DIGITALWRITE_PB4_HIGH;
    CHANGE_CPU_4KHZ;
    // power off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_bod_disable();
    sleep_cpu();
  } else {
    // reduce countdown 5 seconds
    countdown -= 5;
    // blink (18mA)
    CHANGE_CPU_1MHZ;
    for (byte i=0; i<5; i++) {
      DIGITALWRITE_PB1_HIGH;
      DELAY_100MS_1MHZ;
      DIGITALWRITE_PB1_LOW;
    }
    // delay 900ms (11.2mA)
    CHANGE_CPU_250KHZ;
    for (byte i=0; i<9; i++) {
      DELAY_100MS_250KHZ;
    }
    // sleep 4s (6.9mA) and wakeup
    sleepPowerDown();
    sleepWakeup();
  }
}
