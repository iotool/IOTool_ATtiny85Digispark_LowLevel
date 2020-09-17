// Countdown 24 hours for ATtiny85 Lilytiny
// 
// Countdown seconds and tick buildin led at pb1.
// Turn on pb4 und pb1 at the end of countdown.
// Current usage 9.6mA * 24h = 230mAh.
// 
// 250 Byte (4% of 6.012) / RAM 4 Byte

#include "iotool_attiny85lilytiny_lowlevel.h"

static uint32_t countdown;

int main() 
{
  // --- setup ---
  PINMODE_PB1_OUTPUT;
  PINMODE_PB4_OUTPUT;

  countdown = 86400; // 24h * 60m * 60s
  // --- loop ---
  while(1)                // forever
  {
    loop();            
  }
}

void loop() 
{
  // countdown
  if (countdown > 0) {
    CHANGE_CPU_125KHZ;
    DIGITALWRITE_PB4_LOW;
    DIGITALWRITE_PB1_HIGH;
    _delay_ms(5); // 40ms @ 125kHz
    DIGITALWRITE_PB1_LOW;
    _delay_ms(120); // 960ms @ 125kHz
    countdown--;
  }
  
  // finish
  if (countdown == 0) {
    CHANGE_CPU_1MHZ;
    DIGITALWRITE_PB4_HIGH;
    DIGITALWRITE_PB1_HIGH;
  }
}
