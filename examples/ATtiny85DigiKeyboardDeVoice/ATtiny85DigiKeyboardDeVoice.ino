#include "DigiKeyboard.h"

void setup() {
  pinMode(1,OUTPUT);
}

void DigiKeyboardNext() {
  digitalWrite(1,LOW);
  DigiKeyboard.delay(500); // 100
  digitalWrite(1,HIGH);
}

void loop() {
  digitalWrite(1,HIGH);
  /*--- init keyboard ---*/
  DigiKeyboard.sendKeyStroke(0);
  DigiKeyboard.delay(4000);
  /*--- open execute dialog ---*/
  DigiKeyboard.sendKeyStroke(KEY_R, MOD_GUI_LEFT); // Windows+R
  DigiKeyboardNext();
  /*--- run powershell ---*/
  DigiKeyboard.print("powershell");
  DigiKeyboard.delay(500);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext(); 
  DigiKeyboard.delay(4500);
  /*--- use speech api ---*/
  //DE: DigiKeyboard.print("Add-Type -AssemblyName System.speech");
  DigiKeyboard.print("Add/Tzpe /AssemblzName Szstem.speech");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext(); // DigiKeyboard.delay(500); // 100
  //DE: DigiKeyboard.print("$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer");
  DigiKeyboard.print("$speak ) New/Object Szstem.Speech.Sznthesis.SpeechSznthesiyer");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext(); // DigiKeyboard.delay(500); // 100
  //DE: DigiKeyboard.print("$speak.Speak(\"This is a test\")");
  DigiKeyboard.print("$speak.Speak*@This is a test@(");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext(); 
  /*--- exit powershell ---*/
  DigiKeyboard.delay(4500); // 100
  DigiKeyboard.print("exit");
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  DigiKeyboardNext(); // DigiKeyboard.delay(100);
  // DigiKeyboard.sendKeyStroke(KEY_SPACE, MOD_ALT_LEFT);
  // DigiKeyboard.sendKeyStroke(KEY_N);
  digitalWrite(1,LOW);
  for (;;) {/*empty*/}
}
