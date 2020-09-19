// ATtiny85DigisparkDetectPowerbank.ino
// 
// Detect device is running at usb port or powerbank.
// LED will pulse at powerbank and tick at usb port.
// At setup PB4 is 3V at powerbank and 0V at usb port.

static byte DETECT_POWERBANK = 0;

void setup() {
  uint16_t adc;
  adc = analogRead(2); // PB4_ADC2
  if ((adc >= 500) && (adc <= 750)) {
    DETECT_POWERBANK = 1;
  }
  pinMode(1,OUTPUT);
}

void loop() {
  if (DETECT_POWERBANK == 1) {
    for (byte i=0; i<5; i++) {
      digitalWrite(1,HIGH); delay(10); digitalWrite(1,LOW); delay(40);
    }
    digitalWrite(1,HIGH); delay(500); digitalWrite(1,LOW); delay(250);
  } else {
    digitalWrite(1,HIGH);
    delay(100);
    digitalWrite(1,LOW);
    delay(900);
  }
}
