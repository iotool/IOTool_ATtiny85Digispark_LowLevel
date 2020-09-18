// ATtiny85DigisparkBlink.ino
// 
// Test current for different clock speed and voltage input.
// You can reduce the current by using VIN pin, because the voltage regulator 
// reduce the voltage for the microcontroller and the power on led.
// The next option is to reduce the clock speed of the cpu.
// 
// 752 Byte (12% of 6.012) / RAM 0 Byte

#include "iotool_attiny85digispark_lowlevel.h"

int main() 
{
  // --- setup ---
  PINMODE_PB1_OUTPUT;     // pinMode(OUTPUT)
  
  DIGITALWRITE_PB1_HIGH;
  DELAY_1S;
  DIGITALWRITE_PB1_LOW;
  DELAY_1S;

  // --- loop ---
  while(1)                // forever
  {
    loop();            
  }
}

void loop() 
{

  // current: 17.05mA@4.5V(4.5V-5V) 10.10mA/3.2V(4.5V-VIN)
  CHANGE_CPU_1MHZ;
  for (byte i=0; i<10; i++) {
    DIGITALWRITE_PB1_HIGH;
    DELAY_100MS_1MHZ;
    DIGITALWRITE_PB1_LOW;
    DELAY_1S_1MHZ;
  }
  DELAY_1S_1MHZ;
  DELAY_1S_1MHZ;
  DELAY_1S_1MHZ;

  // current: 12.95mA@4.5V(4.5V-5V) 7.41mA/3.2V(4.5V-VIN)
  CHANGE_CPU_500KHZ;
  for (byte i=0; i<10; i++) {
    DIGITALWRITE_PB1_HIGH;
    DELAY_100MS_500KHZ;
    DIGITALWRITE_PB1_LOW;
    DELAY_1S_500KHZ;
  }
  DELAY_1S_500KHZ;
  DELAY_1S_500KHZ;
  DELAY_1S_500KHZ;

  // current: 11.02mA@4.5V(4.5V-5V) 6.12mA/3.2V(4.5V-VIN)
  CHANGE_CPU_250KHZ;
  for (byte i=0; i<10; i++) {
    DIGITALWRITE_PB1_HIGH;
    DELAY_100MS_250KHZ;
    DIGITALWRITE_PB1_LOW;
    DELAY_1S_250KHZ;
  }
  DELAY_1S_250KHZ;
  DELAY_1S_250KHZ;
  DELAY_1S_250KHZ;

  // current: 9.46mA@4.5V(4.5V-5V) 5.18mA/3.2V(4.5V-VIN)
  CHANGE_CPU_125KHZ;
  for (byte i=0; i<10; i++) {
    DIGITALWRITE_PB1_HIGH;
    DELAY_100MS_125KHZ;
    DIGITALWRITE_PB1_LOW;
    DELAY_1S_125KHZ;
  }
  DELAY_1S_125KHZ;
  DELAY_1S_125KHZ;
  DELAY_1S_125KHZ;

  // current: 8.69mA@4.5V(4.5V-5V) 4.68mA/3.2V(4.5V-VIN)
  CHANGE_CPU_62KHZ;
  for (byte i=0; i<10; i++) {
    DIGITALWRITE_PB1_HIGH;
    DELAY_100MS_62KHZ;
    DIGITALWRITE_PB1_LOW;
    DELAY_1S_62KHZ;
  }
  DELAY_1S_62KHZ;
  DELAY_1S_62KHZ;
  DELAY_1S_62KHZ;

  // current: 8.20mA@4.5V(4.5V-5V) 4.41mA/3.2V(4.5V-VIN)
  CHANGE_CPU_31KHZ;
  for (byte i=0; i<10; i++) {
    DIGITALWRITE_PB1_HIGH;
    DELAY_100MS_31KHZ;
    DIGITALWRITE_PB1_LOW;
    DELAY_1S_31KHZ;
  }
  DELAY_1S_31KHZ;
  DELAY_1S_31KHZ;
  DELAY_1S_31KHZ;

}
