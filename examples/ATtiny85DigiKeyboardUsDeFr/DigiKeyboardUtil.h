// DigiKeyboardUtil.h

#define KEYBOARD_LAYOUT_US 1   // qwerty
#define KEYBOARD_LAYOUT_DE 2   // qwertz
#define KEYBOARD_LAYOUT_FR 3   // awerty

void DigiKeyboard_setKeyboardLayout(uint8_t pKeyboardLayout);
void DigiKeyboard_print(String pText);

static uint8_t sKeyboardLayout = KEYBOARD_LAYOUT_US;

void DigiKeyboard_setKeyboardLayout(uint8_t pKeyboardLayout) {
  sKeyboardLayout = pKeyboardLayout;
}

void DigiKeyboard_print(String pText) {
  switch(sKeyboardLayout) {
#ifdef DIGIKEYBOARDDE_H
    case KEYBOARD_LAYOUT_DE:
      DigiKeyboardDe.print(pText);
      break;
#endif //DIGIKEYBOARDDE_H
#ifdef DIGIKEYBOARDFR_H
    case KEYBOARD_LAYOUT_FR:
      DigiKeyboardFr.print(pText);
      break;
#endif //DIGIKEYBOARDFR_H
    default:
      DigiKeyboard.print(pText);
      break;
  }
}
