// Arduino\libraries\TrinketHidCombo\TrinketHidCombo.cpp
// append >>>mod>>

...

//>>>mod>>>
uint16_t mapAsciiToKeyboard(uint8_t ascii) {
  uint16_t  mapping = ascii;
  if (TrinketHidCombo.keyboardMapping == KEYBOARD_DE_QWERTZ) {
    switch(ascii) {
      //   DE    <<<    EN
      case 'y':  mapping='z'; break;
      case 'Y':  mapping='Z'; break;
      case 'z':  mapping='y'; break;
      case 'Z':  mapping='Z'; break;

      case '"':  mapping='@'; break;
      case '^':  mapping='`'; break;
      case '&':  mapping='^'; break;
      case '/':  mapping='&'; break;
      case '(':  mapping='*'; break;
      case ')':  mapping='('; break;
      case '=':  mapping=')'; break;
      case '?':  mapping='_'; break;
      case '`':  mapping='+'; break;
      case '´':  mapping='='; break;
      case '+':  mapping=']'; break;
      case '*':  mapping='}'; break;
      case ';':  mapping='<'; break;
      case ':':  mapping='>'; break;
      case '-':  mapping='/'; break;
      case '_':  mapping='?'; break;

      case '{':  mapping=0x24 | (1<<14); break;   // ALT_RIGHT
      case '}':  mapping=0x27 | (1<<14); break;   // ALT_RIGHT
      case '[':  mapping=0x25 | (1<<14); break;   // ALT_RIGHT
      case ']':  mapping=0x26 | (1<<14); break;   // ALT_RIGHT
      case '@':  mapping=0x14 | (1<<14); break;   // ALT_RIGHT
      case '~':  mapping=0x30 | (1<<14); break;   // ALT_RIGHT
      case '#':  mapping=0x32 | (0xFF<<8); break;
      case '\\': mapping=0x2d | (1<<14); break;   // ALT_RIGHT
      case '<':  mapping=0x64 | (0xFF<<8); break; 
      case '|':  mapping=0x64 | (1<<14); break;   // ALT_RIGHT
      case '>':  mapping=0x64 | (1<<9); break;    // SHIFT_LEFT
      case '\'': mapping=0x32 | (1<<9); break;    // SHIFT_LEFT

      default: break;
    }
  }
  return mapping;
}
//<<<mod<<<

// void ASCII_to_keycode(uint8_t ascii, uint8_t ledState, uint8_t* modifier, uint8_t* keycode)
void ASCII_to_keycode(uint8_t ascii_in, uint8_t ledState, uint8_t* modifier, uint8_t* keycode)
{
	*keycode = 0x00;
	*modifier = 0x00;

//>>>mod>>>
 uint16_t temp = mapAsciiToKeyboard(ascii_in);
 uint8_t ascii = temp & 0xFF;
 uint8_t ascii_keycode = (temp >> 8) & 0xFF;
 if (ascii_keycode > 0) {
   *keycode = ascii;
   if (ascii_keycode != 0xFF) {
     *modifier = ascii_keycode;
   }
   return;
 }
//<<<mod<<<
	
	// see scancode.doc appendix C
	
	if (ascii >= 'A' && ascii <= 'Z')
	{
    ...
