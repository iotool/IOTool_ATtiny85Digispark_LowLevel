// ATtiny85DigiKeyboardNextModePowerbank.ino
// 
// Change mode of Digispark device without hardware modification.
// This example support up to nine different modes / programms.
// 
// To change the mode plug in the device into a powerbank
// and wait until the buildin led turn on permanently, 
// remove the device and plug in again into powerbank, 
// wait until the buildin blinks very fast (next mode saved). 
// Now you can plug in the device into computer usb port.
// 
// run at usb port as keyboard:
// ____________ > ........... > ____________
// bootloader   > show mode   > exec mode     
// - device boot (6000 ms) power led is on
// - optional: show current mode (3000 ms) led blink x times
// - optional: start notepad and display current mode (1500 ms)
// - dummy code (endless loop)
// 
// run at powerbank (a): select next mode
// ____________ > :::::: > ############ > ........... > ############
// bootloader   > init   > next mode    > show mode   > exit setup
// - device boot (6000 ms) power led is on
// - show setup init (1500 ms) led pulse 3x times
// - show select-next (2500 ms) led on [disconnect for next mode]
// - show current mode (3000 ms) led blink x times
// - show setup exit (5000 ms) led on
// 
// run at powerbank (b): confirm next mode
// ____________ > :::::::::::::::::: > ........... > ############
// bootloader   > save next mode     > show mode   > exit setup
// - device boot (6000 ms) power led is on
// - show change mode (2000 ms) led fast blink 40x times
// - show new mode (3000 ms) led blink x times
// - show setup exit (5000 ms) led on


// Use Digispark as keyboard to send data to computer as keyboard input.
// You don't need any serial software like Arduino IDE.
#include "DigiKeyboard.h"

// Use the EEPROM to save the current mode and state of change process.
// There is a limit of 100.000 write ops for eeprom.
#include <avr/eeprom.h>
#define EEPROM_MODE_MIN     1
#define EEPROM_MODE_MAX     9
#define EEPROM_CHANGE_INIT  0xFF
#define EEPROM_CHANGE_YES   0xF0
#define EEPROM_CHANGE_NO    0x00
#define EEPROM_BUFFER_SIZE  2
#define EEPROM_I_MODE       0
#define EEPROM_I_CHANGE     1
#define EEPROM_READ         eeprom_read_block((void*)&eeprom_buffer, (const void*)0, EEPROM_BUFFER_SIZE)
#define EEPROM_WRITE        eeprom_write_block(&eeprom_buffer,(void*)0x00,EEPROM_BUFFER_SIZE)
static uint8_t  eeprom_buffer[EEPROM_BUFFER_SIZE];

// This global flag set durring setup. 
// DETECT_POWERBANK = 0 means device is running at computers usb port (or battery)
// DETECT_POWERBANK = 1 means device is running at powerbank
static byte     DETECT_POWERBANK = 0;

void setup() 
{
  // Detect powerbank by PB4 pin.
  // PB4 = 0V means device is running at computers usb port
  // PB4 = 3V means device is running at powerbank
  uint16_t adc;
  adc = analogRead(2); // PB4_ADC2
  if ((adc >= 500) && (adc <= 750)) {
    DETECT_POWERBANK = 1;
  }

  // Buildin led to display blink codes and values
  // pulsing flashing = setup launched
  // permanently light = disconnect device to change mode
  // flashing several times = display current mode
  // hectic flashing = save new mode
  pinMode(1,OUTPUT);
  
  // Read stored values from the eeprom.
  // Convert initial value (255) to minimum value (1).
  EEPROM_READ;
  if (eeprom_buffer[EEPROM_I_MODE] > EEPROM_MODE_MAX) {
    eeprom_buffer[EEPROM_I_MODE] = EEPROM_MODE_MIN;
  }
}

void loop() 
{
  if (DETECT_POWERBANK == 1) {
    // Processing with recognized powerbank
    loopPowerBank();
  } else {
    // Processing with computer usb port
    loopUsbPort();
  }
}

void loopPowerBank() 
{
  // Start setup, i.e. there is no confirmed change yet.
  // This branch is always called if the device is operated at a powerbank.
  if (!(eeprom_buffer[EEPROM_I_CHANGE] == EEPROM_CHANGE_YES)) {

    // start (+1500 ms = 7500ms)
    // Display a pulsating flashing directly after the boot loader.
    // This flash code signals the start of the setup.
    for (byte k=0; k<3; k++) {
      for (byte i=0; i<3; i++) {
        digitalWrite(1,HIGH); 
        delay(10); 
        digitalWrite(1,LOW); 
        delay(30);
      }
      digitalWrite(1,HIGH); 
      delay(280); 
      digitalWrite(1,LOW); 
      delay(100);
    }

    // change = yes (+2200 ms = 9700ms)
    // Save the change YES in the EEPROM and the built-in LED lights up permanently.
    // If now the device is removed, then at the next start change = YES is recognized.
    digitalWrite(1,HIGH); 
    eeprom_buffer[EEPROM_I_CHANGE] = EEPROM_CHANGE_YES;
    EEPROM_WRITE;
    delay(2200);

    // change = yes (+300 ms = 10000ms)
    // Save the change NO in the EEPROM and turn the built-in LED off.
    // If now the device is removed, then at the next start change = NO is recognized.
    digitalWrite(1,LOW); 
    eeprom_buffer[EEPROM_I_CHANGE] = EEPROM_CHANGE_NO;
    EEPROM_WRITE;
    delay(300);

    // display mode (+3000 ms = 13000ms)
    // Display the current number of the mode by flashing briefly.
    for (byte i=0; i<eeprom_buffer[EEPROM_I_MODE]; i++) {
      digitalWrite(1,HIGH); 
      delay(100); 
      digitalWrite(1,LOW); 
      delay(200);
    }

    // exit
    // Indicate the end of the setup by lighting up the built-in LED.
    // The powerbank usually switches off after 15 seconds if too little current is drawn.
    delay(500); 
    digitalWrite(1,HIGH); 
    delay(4500); 
    digitalWrite(1,LOW); 
    while(1) {}
  }

  // Confirm setup, i.e. the device was removed from the Powerbank during the confirmation phase.
  // Confirmation prevents accidentally changing the mode.  
  else {

    // save eeprom
    // Only by restarting the powerbank the change is confirmed. 
    // In case of an overflow, the mode starts over again.
    // The change status is reset to 0xFF to reduce the number of solutions from EEPROM.
    eeprom_buffer[EEPROM_I_MODE]++;
    if (eeprom_buffer[EEPROM_I_MODE] > EEPROM_MODE_MAX) {
      eeprom_buffer[EEPROM_I_MODE] = EEPROM_MODE_MIN;
    }
    eeprom_buffer[EEPROM_I_CHANGE] = EEPROM_CHANGE_INIT;
    EEPROM_WRITE;

    // start (+2000 ms = 7000ms)
    // Display a fast flashing after saving to eeprom.
    // This flash code signals the save process of the setup.
    for (byte k=0; k<40; k++) {
      digitalWrite(1,HIGH); 
      delay(10); 
      digitalWrite(1,LOW); 
      delay(40);
    }
    
    // display (+3000 ms = 13000ms)
    // Display the current number of the mode by flashing briefly.
    for (byte i=0; i<eeprom_buffer[EEPROM_I_MODE]; i++) {
      digitalWrite(1,HIGH); 
      delay(100); 
      digitalWrite(1,LOW); 
      delay(200);
    }

    // exit
    // Indicate the end of the setup by lighting up the built-in LED.
    // The powerbank usually switches off after 15 seconds if too little current is drawn.
    delay(500); 
    digitalWrite(1,HIGH); 
    delay(4500); 
    digitalWrite(1,LOW); 
    while(1) {}
  }
}

// Flag to run through the start procedure only once.
static uint8_t flag_start = 0;

void loopUsbPort() 
{
  // Send the first keystroke to the computer to establish communication.
  DigiKeyboard.sendKeyStroke(0);

  // Execution once when the mode is started.
  if (flag_start == 0) {
    flag_start = 1;

    // ignore change mode
    // Initialize change flag, if necessary.
    if (!(eeprom_buffer[EEPROM_I_CHANGE] == EEPROM_CHANGE_INIT)) {
      eeprom_buffer[EEPROM_I_CHANGE] = EEPROM_CHANGE_INIT;
      EEPROM_WRITE;
      digitalWrite(1,HIGH); 
      DigiKeyboard.delay(500); 
      digitalWrite(1,LOW); 
      DigiKeyboard.delay(500);
    }
    
    // display mode (3000 ms)
    // Optional: the display of the current mode can also be omitted.
    for (byte i=0; i<eeprom_buffer[EEPROM_I_MODE]; i++) {
      digitalWrite(1,HIGH); 
      DigiKeyboard.delay(100); 
      digitalWrite(1,LOW); 
      DigiKeyboard.delay(200);
    }
    for (byte i=eeprom_buffer[EEPROM_I_MODE]-1; i<EEPROM_MODE_MAX; i++) {
      DigiKeyboard.delay(300); 
    }

    // debug
    // Optional: the display of the current mode can also be omitted.
    DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // Windows+R
    DigiKeyboard.delay(500);
    DigiKeyboard.print("notepad");
    DigiKeyboard.delay(500);
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    DigiKeyboard.delay(500);
    DigiKeyboard.print("mode ");
    uint16_t mode = eeprom_buffer[EEPROM_I_MODE];
    DigiKeyboard.print(mode);
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
  }

  // regular processing ... add here
  digitalWrite(1,HIGH);
  DigiKeyboard.delay(100);
  digitalWrite(1,LOW);
  DigiKeyboard.delay(900);
}
