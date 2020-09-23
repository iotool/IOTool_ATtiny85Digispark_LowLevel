
// #define __AVR_ATtiny85__
// #include "DigiSleep.h"
// #include "DigiKeyboardMouse.h"                  // qwerty (2.484 Byte)
// 
// gRebootMemory.mode = REBOOT_MODE_KEYBOARD;
// gRebootMemory.mode = REBOOT_MODE_MOUSE;

/* --- define mode --- */


#ifndef __DigiKeyboardMouseHead_h__
#define __DigiKeyboardMouseHead_h__

#define REBOOT_INIT           0xABCD
#define REBOOT_MODE_KEYBOARD  0xFA
#define REBOOT_MODE_MOUSE     0xAF

typedef struct {
  uint16_t init;
  uint8_t  count;
  uint8_t  mode;
  uint8_t  next;
} tRebootMemory;
tRebootMemory gRebootMemory __attribute__
  ((__section__ (".noinit")))
;

void initRebootMemory() {
  if (gRebootMemory.init != REBOOT_INIT) {
    gRebootMemory.init = REBOOT_INIT;
    gRebootMemory.count = 0;
    gRebootMemory.mode = REBOOT_MODE_KEYBOARD;
    gRebootMemory.next = REBOOT_MODE_KEYBOARD;
  } else {
    gRebootMemory.count += 1;
  }
}

void reboot(void) {
  if (gRebootMemory.mode != gRebootMemory.next) {
   gRebootMemory.mode = gRebootMemory.next;
  }
  noInterrupts(); // disable interrupts which could mess with changing prescaler
  CLKPR = 0b10000000; // enable prescaler speed change
  CLKPR = 0; // set prescaler to default (16mhz) mode required by bootloader
  void (*ptrToFunction)(); // allocate a function pointer
  ptrToFunction = 0x0000; // set function pointer to bootloader reset vector
  (*ptrToFunction)(); // jump to reset, which bounces in to bootloader
}

#endif // __DigiKeyboardMouseHead_h__

/* --- DigiKeyboard.h --- */

/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 * Modified for Digispark by Digistump
 */
#ifndef __DigiKeyboard_h__
#define __DigiKeyboard_h__

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>

#include "usbdrv.h"
#include "scancode-ascii-table.h"

// TODO: Work around Arduino 12 issues better.
//#include <WConstants.h>
//#undef int()

typedef uint8_t byte;


#define BUFFER_SIZE 2 // Minimum of 2: 1 for modifiers + 1 for keystroke 


static uchar    idleRate;           // in 4 ms units 


/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and but we do allow
 * simultaneous key presses. 
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */
const PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { /* USB report descriptor */
  0x05, 0x01,                    // USAGE_PAGE (Generic Desktop) 
  0x09, 0x06,                    // USAGE (Keyboard) 
  0xa1, 0x01,                    // COLLECTION (Application) 
  0x05, 0x07,                    //   USAGE_PAGE (Keyboard) 
  0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl) 
  0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI) 
  0x15, 0x00,                    //   LOGICAL_MINIMUM (0) 
  0x25, 0x01,                    //   LOGICAL_MAXIMUM (1) 
  0x75, 0x01,                    //   REPORT_SIZE (1) 
  0x95, 0x08,                    //   REPORT_COUNT (8) 
  0x81, 0x02,                    //   INPUT (Data,Var,Abs) 
  0x95, 0x01,           //   REPORT_COUNT (simultaneous keystrokes) 
  0x75, 0x08,                    //   REPORT_SIZE (8) 
  0x25, 0x65,                    //   LOGICAL_MAXIMUM (101) 
  0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated)) 
  0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application) 
  0x81, 0x00,                    //   INPUT (Data,Ary,Abs) 
  0xc0                           // END_COLLECTION 
};



/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */
#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_A       4
#define KEY_B       5
#define KEY_C       6
#define KEY_D       7
#define KEY_E       8
#define KEY_F       9
#define KEY_G       10
#define KEY_H       11
#define KEY_I       12
#define KEY_J       13
#define KEY_K       14
#define KEY_L       15
#define KEY_M       16
#define KEY_N       17
#define KEY_O       18
#define KEY_P       19
#define KEY_Q       20
#define KEY_R       21
#define KEY_S       22
#define KEY_T       23
#define KEY_U       24
#define KEY_V       25
#define KEY_W       26
#define KEY_X       27
#define KEY_Y       28
#define KEY_Z       29
#define KEY_1       30
#define KEY_2       31
#define KEY_3       32
#define KEY_4       33
#define KEY_5       34
#define KEY_6       35
#define KEY_7       36
#define KEY_8       37
#define KEY_9       38
#define KEY_0       39

#define KEY_ENTER   40

#define KEY_SPACE   44

#define KEY_F1      58
#define KEY_F2      59
#define KEY_F3      60
#define KEY_F4      61
#define KEY_F5      62
#define KEY_F6      63
#define KEY_F7      64
#define KEY_F8      65
#define KEY_F9      66
#define KEY_F10     67
#define KEY_F11     68
#define KEY_F12     69

#define KEY_ARROW_LEFT 0x50


class DigiKeyboardDevice : public Print {
 public:
  DigiKeyboardDevice () {
    cli();
    usbDeviceDisconnect();
    _delay_ms(250);
    usbDeviceConnect();


    usbInit();
      
    sei();

    // TODO: Remove the next two lines once we fix
    //       missing first keystroke bug properly.
    memset(reportBuffer, 0, sizeof(reportBuffer));      
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
    
  void update() {
    usbPoll();
  }
	
	// delay while updating until we are finished delaying
	void delay(long milli) {
		unsigned long last = millis();
	  while (milli > 0) {
	    unsigned long now = millis();
	    milli -= now - last;
	    last = now;
	    update();
	  }
	}
  
  //sendKeyStroke: sends a key press AND release
  void sendKeyStroke(byte keyStroke) {
    sendKeyStroke(keyStroke, 0);
  }

  //sendKeyStroke: sends a key press AND release with modifiers
  void sendKeyStroke(byte keyStroke, byte modifiers) {
	sendKeyPress(keyStroke, modifiers);
    // This stops endlessly repeating keystrokes:
	sendKeyPress(0,0);
  }

  //sendKeyPress: sends a key press only - no release
  //to release the key, send again with keyPress=0
  void sendKeyPress(byte keyPress) {
	sendKeyPress(keyPress, 0);
  }

  //sendKeyPress: sends a key press only, with modifiers - no release
  //to release the key, send again with keyPress=0
  void sendKeyPress(byte keyPress, byte modifiers) {
   	while (!usbInterruptIsReady()) {
      // Note: We wait until we can send keyPress
      //       so we know the previous keyPress was
      //       sent.
    	usbPoll();
    	_delay_ms(5);
    }
    
    memset(reportBuffer, 0, sizeof(reportBuffer));
		
    reportBuffer[0] = modifiers;
    reportBuffer[1] = keyPress;
    
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }
  
  size_t write(uint8_t chr) {
    uint8_t data = pgm_read_byte_near(ascii_to_scan_code_table + (chr - 8));
    sendKeyStroke(data & 0b01111111, data >> 7 ? MOD_SHIFT_RIGHT : 0);
    return 1;
  }
    
  //private: TODO: Make friend?
  uchar    reportBuffer[2];    // buffer for HID reports [ 1 modifier byte + (len-1) key strokes]
  using Print::write;
};

DigiKeyboardDevice DigiKeyboard = DigiKeyboardDevice();

//-#ifdef __cplusplus
//-extern "C"{
//-#endif 
//-  // USB_PUBLIC uchar usbFunctionSetup
//-	uchar usbFunctionSetup(uchar data[8]) {
//-    usbRequest_t    *rq = (usbRequest_t *)((void *)data);
//-
//-    usbMsgPtr = DigiKeyboard.reportBuffer; //
//-    if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
//-      /* class request type */
//-
//-      if (rq->bRequest == USBRQ_HID_GET_REPORT) {
//-				/* wValue: ReportType (highbyte), ReportID (lowbyte) */
//-
//-				/* we only have one report type, so don't look at wValue */
//-        // TODO: Ensure it's okay not to return anything here?    
//-				return 0;
//-
//-      } else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
//-				//usbMsgPtr = &idleRate;
//-				//return 1;
//-				return 0;
//-				
//-      } else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
//-				idleRate = rq->wValue.bytes[1];
//-				
//-      }
//-    } else {
//-      /* no vendor specific requests implemented */
//-    }
//-		
//-    return 0;
//-  }
//-#ifdef __cplusplus
//-} // extern "C"
//-#endif

#endif // __DigiKeyboard_h__



/* --- DigiMouse.h --- */

/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * TODO: Make a proper file header. :-)
 * Modified for Digispark by Digistump
 * And now modified by Sean Murphy (duckythescientist) from a keyboard device to a mouse device
 * Most of the credit for the joystick code should go to Raphaël Assénat
 * And now mouse credit is due to Yiyin Ma and Abby Lin of Cornell
 */
#ifndef __DigiMouse_h__
#define __DigiMouse_h__
 
#define REPORT_SIZE 4

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "usbdrv.h"
//#include "devdesc.h"
#include "oddebug.h"
#include "usbconfig.h"

static const uchar *rt_usbHidReportDescriptor = NULL;
static uchar rt_usbHidReportDescriptorSize = 0;
static const uchar *rt_usbDeviceDescriptor = NULL;
static uchar rt_usbDeviceDescriptorSize = 0;

#define MOUSEBTN_LEFT_MASK		0x01
#define MOUSEBTN_RIGHT_MASK		0x02
#define MOUSEBTN_MIDDLE_MASK	0x04

// TODO: Work around Arduino 12 issues better.
//#include <WConstants.h>
//#undef int()

typedef uint8_t byte;

/* What was most recently read from the controller */
static unsigned char last_built_report[REPORT_SIZE];

/* What was most recently sent to the host */
static unsigned char last_sent_report[REPORT_SIZE];

uchar		 reportBuffer[REPORT_SIZE];

// report frequency set to default of 50hz
#define DIGIMOUSE_DEFAULT_REPORT_INTERVAL 20
static unsigned char must_report = 0;
static unsigned char idle_rate = DIGIMOUSE_DEFAULT_REPORT_INTERVAL / 4; // in units of 4ms
// new minimum report frequency system:
static unsigned long last_report_time = 0;


char usb_hasCommed = 0;

const PROGMEM unsigned char mouse_usbHidReportDescriptor[] = { /* USB report descriptor */
		0x05, 0x01,										 // USAGE_PAGE (Generic Desktop)
		0x09, 0x02,										 // USAGE (Mouse)
		0xa1, 0x01,										 // COLLECTION (Application)
		0x09, 0x01,										 //		USAGE_PAGE (Pointer)
		0xa1, 0x00,										 //		COLLECTION (Physical)
		0x05, 0x09,										 //		USAGE_PAGE (Button)
		0x19, 0x01,										 //		USAGE_MINIMUM (Button 1)
		0x29, 0x03,										 //		USAGE_MAXIMUM (Button 3)
		0x15, 0x00,										 //		LOGICAL_MINIMUM (0)
		0x25, 0x01,										 //		LOGICAL_MAXIMUM (1)
		0x95, 0x03,										 //		REPORT_COUNT (3)
		0x75, 0x01,										 //		REPORT_SIZE (1)
		0x81, 0x02,										 //		INPUT (Data,Var,Abs)
		0x95, 0x01,										 //		REPORT_COUNT (1)
		0x75, 0x05,										 //		REPORT_SIZE (5)
		0x81, 0x01,										 //		Input(Cnst)
		0x05, 0x01,										 //		USAGE_PAGE(Generic Desktop)
		0x09, 0x30,										 //		USAGE(X)
		0x09, 0x31,										 //		USAGE(Y)
		0x15, 0x81,										 //		LOGICAL_MINIMUM (-127)
		0x25, 0x7f,										 //		LOGICAL_MAXIMUM (127)
		0x75, 0x08,										 //		REPORT_SIZE (8)
		0x95, 0x02,										 //		REPORT_COUNT (2)
		0x81, 0x06,										 //		INPUT (Data,Var,Rel)
		0x09, 0x38,											//	 Usage (Wheel)
		0x95, 0x01,											//	 Report Count (1),
		0x81, 0x06,											//	 Input (Data, Variable, Relative)
		0xc0,														// END_COLLECTION
		0xc0													 // END_COLLECTION
};


#define USBDESCR_DEVICE					1

const unsigned char usbDescrDevice[] PROGMEM = {		/* USB device descriptor */
		18,					/* sizeof(usbDescrDevice): length of descriptor in bytes */
		USBDESCR_DEVICE,		/* descriptor type */
		0x01, 0x01, /* USB version supported */
		USB_CFG_DEVICE_CLASS,
		USB_CFG_DEVICE_SUBCLASS,
		0,					/* protocol */
		8,					/* max packet size */
		USB_CFG_VENDOR_ID,	/* 2 bytes */
		USB_CFG_DEVICE_ID,	/* 2 bytes */
		USB_CFG_DEVICE_VERSION, /* 2 bytes */
#if USB_CFG_VENDOR_NAME_LEN
		1,					/* manufacturer string index */
#else
		0,					/* manufacturer string index */
#endif
#if USB_CFG_DEVICE_NAME_LEN
		2,					/* product string index */
#else
		0,					/* product string index */
#endif
#if USB_CFG_SERIAL_NUMBER_LENGTH
		3,					/* serial number string index */
#else
		0,					/* serial number string index */
#endif
		1,					/* number of configurations */
};


void buildReport(unsigned char *reportBuf) {
	if (reportBuf != NULL) {
		memcpy(reportBuf, last_built_report, REPORT_SIZE);
	}
	
	memcpy(last_sent_report, last_built_report, REPORT_SIZE); 
}

void clearMove() {
	// because we send deltas in movement, so when we send them, we clear them
	last_built_report[1] = 0;
	last_built_report[2] = 0;
	last_built_report[3] = 0;
	last_sent_report[1] = 0;
	last_sent_report[2] = 0;
	last_sent_report[3] = 0;
}
	




 
class DigiMouseDevice {
 public:
	DigiMouseDevice () {

		rt_usbHidReportDescriptor = mouse_usbHidReportDescriptor;
		rt_usbHidReportDescriptorSize = sizeof(mouse_usbHidReportDescriptor);
		rt_usbDeviceDescriptor = usbDescrDevice;
		rt_usbDeviceDescriptorSize = sizeof(usbDescrDevice);
	}

	void begin() {
		cli();
		usbDeviceDisconnect();
		_delay_ms(200);
		usbDeviceConnect();	
		
		usbInit();
		
		sei();
		last_report_time = millis();
	}
	

	
	void refresh() {
		update();
	}

	void poll() {
		update();
	}

	
	void update() {
		usbPoll();
		
		// instead of above code, use millis arduino system to enforce minimum reporting frequency
		unsigned long time_since_last_report = millis() - last_report_time;
		if (time_since_last_report >= (idle_rate * 4 /* in units of 4ms - usb spec stuff */)) {
			last_report_time += idle_rate * 4;
			must_report = 1;
		}
		
		// if the report has changed, try force an update anyway
		if (memcmp(last_built_report, last_sent_report, REPORT_SIZE)) {
			must_report = 1;
		}
		
		// if we want to send a report, signal the host computer to ask us for it with a usb 'interrupt'
		if (must_report) {
			if (usbInterruptIsReady()) {
				must_report = 0;
				buildReport(reportBuffer); // put data into reportBuffer
				clearMove(); // clear deltas
				usbSetInterrupt(reportBuffer, REPORT_SIZE);
			}
		}
	}
	
	// delay while updating until we are finished
	void delay(long milli) {
		unsigned long last = millis();
	  while (milli > 0) {
	    unsigned long now = millis();
	    milli -= now - last;
	    last = now;
	    update();
	  }
	}
	
	void moveX(char deltaX)	{
		if (deltaX == -128) deltaX = -127;
		last_built_report[1] = *(reinterpret_cast<unsigned char *>(&deltaX));
	}
	
	void moveY(char deltaY) {
		if (deltaY == -128) deltaY = -127;
		last_built_report[2] = *(reinterpret_cast<unsigned char *>(&deltaY));
	}
	
	void scroll(char deltaS)	{
		if (deltaS == -128) deltaS = -127;
		last_built_report[3] = *(reinterpret_cast<unsigned char *>(&deltaS));	 
	}
	
	void move(char deltaX, char deltaY, char deltaS) {
		if (deltaX == -128) deltaX = -127;
		if (deltaY == -128) deltaY = -127;
		if (deltaS == -128) deltaS = -127;
		last_built_report[1] = *(reinterpret_cast<unsigned char *>(&deltaX));
		last_built_report[2] = *(reinterpret_cast<unsigned char *>(&deltaY));
		last_built_report[3] = *(reinterpret_cast<unsigned char *>(&deltaS));
	}

	void move(char deltaX, char deltaY, char deltaS, char buttons) {
		if (deltaX == -128) deltaX = -127;
		if (deltaY == -128) deltaY = -127;
		if (deltaS == -128) deltaS = -127;
		last_built_report[0] = buttons;
		last_built_report[1] = *(reinterpret_cast<unsigned char *>(&deltaX));
		last_built_report[2] = *(reinterpret_cast<unsigned char *>(&deltaY));
		last_built_report[3] = *(reinterpret_cast<unsigned char *>(&deltaS));
	}

	void rightClick(){
		last_built_report[0] = MOUSEBTN_RIGHT_MASK;
	}

	void leftClick(){
		last_built_report[0] = MOUSEBTN_RIGHT_MASK;
	}
	
	void middleClick(){
		last_built_report[0] = MOUSEBTN_RIGHT_MASK;
	}
	
	void setButtons(unsigned char buttons) {
		last_built_report[0] = buttons;
	}
	
	void setValues(unsigned char values[]) {
		memcpy(last_built_report, values, REPORT_SIZE);
	}
	
	//private: TODO: Make friend?
	// what does this even mean? -- Bluebie
};

// create the global singleton DigiMouse
DigiMouseDevice DigiMouse = DigiMouseDevice();


#ifdef __cplusplus
extern "C"{
#endif 
	// USB_PUBLIC uchar usbFunctionSetup
	
//-	uchar usbFunctionSetup(uchar data[8]) {
//-		usbRequest_t *rq = (usbRequest_t *)data;
//-	
//-		usbMsgPtr = reportBuffer;
//-	
//-		if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {		/* class request type */
//-			if (rq->bRequest == USBRQ_HID_GET_REPORT) {	 /* wValue: ReportType (highbyte), ReportID (lowbyte) */
//-				/* we only have one report type, so don't look at wValue */
//-				//curGamepad->buildReport(reportBuffer);
//-				//return curGamepad->report_size;
//-				return REPORT_SIZE;
//-			} else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
//-				usbMsgPtr = &idle_rate;
//-				return 1;
//-			} else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
//-				idle_rate = rq->wValue.bytes[1];
//-			}
//-		} else {
//-			/* no vendor specific requests implemented */
//-		}
//-		return 0;
//-	}
	
//# uchar	usbFunctionDescriptor(struct usbRequest *rq) {
//# 		if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_STANDARD) {
//#  		return 0;
//# 		}
//# 	
//# 	if (rq->bRequest == USBRQ_GET_DESCRIPTOR) {
//# 		// USB spec 9.4.3, high byte is descriptor type
//# 		switch (rq->wValue.bytes[1]) {
//# 			case USBDESCR_DEVICE:
//# 				usbMsgPtr = rt_usbDeviceDescriptor;
//# 				return rt_usbDeviceDescriptorSize;
//# 				break;
//# 
//# 			case USBDESCR_HID_REPORT:
//# 				usbMsgPtr = rt_usbHidReportDescriptor;
//# 				return rt_usbHidReportDescriptorSize;
//# 				break;
//# 		}
//# 	}
//# 
//# 	return 0;
//# }

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __DigiMouse_h__

/* --- combi keyboard and mouse --- */

#ifndef __DigiKeyboardMouseBody_h__
#define __DigiKeyboardMouseBody_h__

#ifdef __cplusplus
extern "C"{
#endif 
  // USB_PUBLIC uchar usbFunctionSetup
	uchar usbFunctionSetup(uchar data[8]) {
	 
	 if (gRebootMemory.mode != REBOOT_MODE_MOUSE) 
	 {

    usbRequest_t    *rq = (usbRequest_t *)((void *)data);

    usbMsgPtr = DigiKeyboard.reportBuffer; //
    if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
      /* class request type */

      if (rq->bRequest == USBRQ_HID_GET_REPORT) {
				/* wValue: ReportType (highbyte), ReportID (lowbyte) */

				/* we only have one report type, so don't look at wValue */
        // TODO: Ensure it's okay not to return anything here?    
				return 0;

      } else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
				//usbMsgPtr = &idleRate;
				//return 1;
				return 0;
				
      } else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
				idleRate = rq->wValue.bytes[1];
				
      }
    } else {
      /* no vendor specific requests implemented */
    }
		
    return 0; 

	 } // gRebootMemory.mode != REBOOT_MODE_MOUSE

	 if (gRebootMemory.mode == REBOOT_MODE_MOUSE) 
	 {
  		usbRequest_t *rq = (usbRequest_t *)data;
  	
  		usbMsgPtr = reportBuffer;
  	
  		if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {		/* class request type */
  			if (rq->bRequest == USBRQ_HID_GET_REPORT) {	 /* wValue: ReportType (highbyte), ReportID (lowbyte) */
  				/* we only have one report type, so don't look at wValue */
  				//curGamepad->buildReport(reportBuffer);
  				//return curGamepad->report_size;
  				return REPORT_SIZE;
  			} else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
  				usbMsgPtr = &idle_rate;
  				return 1;
  			} else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
  				idle_rate = rq->wValue.bytes[1];
  			}
  		} else {
  			/* no vendor specific requests implemented */
  		}
  		return 0;
	 } // gRebootMemory.mode == REBOOT_MODE_MOUSE

 }
#ifdef __cplusplus
} // extern "C"
#endif

void delayKeyboardMouse(long millis) {
  if (gRebootMemory.mode == REBOOT_MODE_MOUSE) {
    DigiMouse.delay(millis);
  } else {
    DigiKeyboard.delay(millis);
  }
}

#endif // __DigiKeyboardMouseBody_h__
