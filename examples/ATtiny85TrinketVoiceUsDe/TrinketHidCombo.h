// \Arduino\libraries\TrinketHidCombo\TrinketHidCombo.h
// append >>>mod>>>


#ifndef _TRINKETHIDCOMBO_H_
#define _TRINKETHIDCOMBO_H_

...

class Trinket_Hid_Combo : public Print
{
	private:
	public:
		Trinket_Hid_Combo(); // empty constructor, ignore me
    ...
    
  //>>>mod>>>
  uint8_t keyboardMapping = 0;
  //<<<mod<<<
};

...

//>>>mod>>>
// keyboard ascii mapping
#define KEYBOARD_US_QWERTY     0x00
#define KEYBOARD_DE_QWERTZ     0x01
//<<<mod<<<

#endif
