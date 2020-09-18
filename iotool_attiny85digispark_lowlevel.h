// iotool_attiny85digispark_lowlevel.h
// 
// Low level api to program ATtiny85 Lilytiny at Arduino IDE 1.6.x
// to reduce the binary code (maximum 6012 bytes with CJMCU bootloader).
// 
// https://github.com/iotool/IOTool_ATtiny85lilytiny_LowLevel
// 
//      A0/PB5 (1)  (8) VCC
// VRef A3/PB3 (2)  (7) PB2/A1  
//      A2/PB4 (3)  (6) PB1/PWM LED
//         GND (4)  (5) PB0/PWM
// 
// Lilytiny use a special bootloader and hardware circuit for PB3 + PB4
// to upload sketch via usb without serial chip (CJMCU bootloader).
// For this reason it takes 6 seconds after power on until main starts.
// Install Digistump.Drivers at Windows and Board at Arduino 1.6.8 IDE
// choise board Digispark (Default 16.5 MHz), DISCONNECT usb cable,
// sketch > upload, wait until prompt "Plug in device now...",
// plug in usb cable and the sketch will upload to Lilytiny.
// The disconnect/connect needed to start the mcu (reset not avalable).
// 
// 2020-09-17  RoHa  v1.0 port of ATtiny13a

#ifndef IOTOOL_ATTINY85DIGISPARK_LOWLEVEL_H_
#define IOTOOL_ATTINY85DIGISPARK_LOWLEVEL_H_

#include <avr/io.h>          // low level pinmode
#include <util/delay.h>      // low level delay
#include <avr/interrupt.h>   // timer interrupt for sleep
#define __AVR_ATtiny85__     // workaround Arduino IDE 1.6.8
#include "iotool_attiny85digispark_sleep.h"
#include <avr/sleep.h>       // sleep to reduce power
#include <avr/wdt.h>
#include <avr/power.h>

#define CHANGE_CPU_1MHZ   cli();CLKPR=(1<<CLKPCE);CLKPR=0;sei(); // :1  17.05mA/4.5V 10.10mA/3.2V default 
#define CHANGE_CPU_500KHZ cli();CLKPR=(1<<CLKPCE);CLKPR=1;sei(); // :2  12.95mA/4.5V  7.41mA/3.2V
#define CHANGE_CPU_250KHZ cli();CLKPR=(1<<CLKPCE);CLKPR=2;sei(); // :4  11.02mA/4.5V  6.12mA/3.2V
#define CHANGE_CPU_125KHZ cli();CLKPR=(1<<CLKPCE);CLKPR=3;sei(); // :8   9.46mA/4.5V  5.18mA/3.2V
#define CHANGE_CPU_62KHZ  cli();CLKPR=(1<<CLKPCE);CLKPR=4;sei(); // :16  8.69mA/4.5V  4.68mA/3.2V
#define CHANGE_CPU_31KHZ  cli();CLKPR=(1<<CLKPCE);CLKPR=5;sei(); // :32  8.20mA/4.5V  4.41mA/3.2V
#define CHANGE_CPU_16KHZ  cli();CLKPR=(1<<CLKPCE);CLKPR=6;sei(); // :64  7.65mA/4.5V  4.17mA/3.2V
#define CHANGE_CPU_8KHZ   cli();CLKPR=(1<<CLKPCE);CLKPR=7;sei(); // :128 7.56mA/4.5V  4.12mA/3.2V
#define CHANGE_CPU_4KHZ   cli();CLKPR=(1<<CLKPCE);CLKPR=8;sei(); // :256 7.55mA/4.5V  4.11mA/3.2V

// pinMode(PBx,INPUT/OUTPUT)
#define PINMODE_PB0_INPUT   DDRB &= ~(1 << PB0)
#define PINMODE_PB1_INPUT   DDRB &= ~(1 << PB1)
#define PINMODE_PB2_INPUT   DDRB &= ~(1 << PB2)
#define PINMODE_PB3_INPUT   DDRB &= ~(1 << PB3)
#define PINMODE_PB4_INPUT   DDRB &= ~(1 << PB4)
#define PINMODE_PB5_INPUT   DDRB &= ~(1 << PB5)
#define PINMODE_PB0_OUTPUT  DDRB |= (1 << PB0)
#define PINMODE_PB1_OUTPUT  DDRB |= (1 << PB1)
#define PINMODE_PB2_OUTPUT  DDRB |= (1 << PB2)
#define PINMODE_PB3_OUTPUT  DDRB |= (1 << PB3)
#define PINMODE_PB4_OUTPUT  DDRB |= (1 << PB4)
#define PINMODE_PB5_OUTPUT  DDRB |= (1 << PB5)

// digitalWrite(PBx,LOW/HIGH)
#define DIGITALWRITE_PB0_LOW     PORTB &= ~(1 << PB0)
#define DIGITALWRITE_PB1_LOW     PORTB &= ~(1 << PB1)
#define DIGITALWRITE_PB2_LOW     PORTB &= ~(1 << PB2)
#define DIGITALWRITE_PB3_LOW     PORTB &= ~(1 << PB3)
#define DIGITALWRITE_PB4_LOW     PORTB &= ~(1 << PB4)
#define DIGITALWRITE_PB5_LOW     PORTB &= ~(1 << PB5)
#define DIGITALWRITE_PB0_HIGH    PORTB |= (1 << PB0)
#define DIGITALWRITE_PB1_HIGH    PORTB |= (1 << PB1)
#define DIGITALWRITE_PB2_HIGH    PORTB |= (1 << PB2)
#define DIGITALWRITE_PB3_HIGH    PORTB |= (1 << PB3)
#define DIGITALWRITE_PB4_HIGH    PORTB |= (1 << PB4)
#define DIGITALWRITE_PB5_HIGH    PORTB |= (1 << PB5)
#define DIGITALWRITE_PB0_TOGGLE  PORTB ^= (1 << PB0)
#define DIGITALWRITE_PB1_TOGGLE  PORTB ^= (1 << PB1)
#define DIGITALWRITE_PB2_TOGGLE  PORTB ^= (1 << PB2)
#define DIGITALWRITE_PB3_TOGGLE  PORTB ^= (1 << PB3)
#define DIGITALWRITE_PB4_TOGGLE  PORTB ^= (1 << PB4)
#define DIGITALWRITE_PB5_TOGGLE  PORTB ^= (1 << PB5)

// delay @ 1.0 MHz
#define DELAY_1MS    _delay_ms(1)
#define DELAY_5MS    _delay_ms(5)
#define DELAY_10MS   _delay_ms(10)
#define DELAY_50MS   _delay_ms(50)
#define DELAY_100MS  _delay_ms(100)
#define DELAY_320MS  _delay_ms(320)
#define DELAY_500MS  _delay_ms(500)
#define DELAY_1S     _delay_ms(1000)

// delay @ xy MHz
#define DELAY_1S_1MHZ       _delay_ms(1000)
#define DELAY_1S_500KHZ     _delay_ms(500)
#define DELAY_1S_250KHZ     _delay_ms(250)
#define DELAY_1S_125KHZ     _delay_ms(125)
#define DELAY_1S_62KHZ      _delay_ms(62)
#define DELAY_1S_31KHZ      _delay_ms(31)
#define DELAY_100MS_1MHZ    _delay_ms(100)
#define DELAY_100MS_500KHZ  _delay_ms(50)
#define DELAY_100MS_250KHZ  _delay_ms(25)
#define DELAY_100MS_125KHZ  _delay_ms(10)
#define DELAY_100MS_62KHZ   _delay_ms(6)
#define DELAY_100MS_31KHZ   _delay_ms(3)
#define DELAY_33MS_1MHZ     _delay_ms(33)
#define DELAY_33MS_500KHZ   _delay_ms(17)
#define DELAY_33MS_250KHZ   _delay_ms(8)
#define DELAY_33MS_125KHZ   _delay_ms(3)
#define DELAY_33MS_62KHZ    _delay_ms(2)
#define DELAY_33MS_31KHZ    _delay_ms(1)

#endif // IOTOOL_ATTINY85DIGISPARK_LOWLEVEL_H_
