// ATtiny85DigisparkCurrent.ino

void setup() {
  // Lilytiny 20.8mA / BadUSB 18.5mA
}

void blink(byte i) {
  pinMode(1,OUTPUT);
  while(i>=1) {
    i--;
    digitalWrite(1,HIGH);
    delay(100);
    digitalWrite(1,LOW);
    delay(900);
  }
}

void loop() {
  
  // Lilytiny 21.9mA (+1.1mA) / BadUSB 19.3mA (+0.8mA)
  blink(1);
  pinMode(1,OUTPUT); digitalWrite(1,LOW);
  pinMode(3,INPUT);  digitalWrite(3,LOW);
  pinMode(4,INPUT);  digitalWrite(4,LOW);
  pinMode(0,INPUT);  digitalWrite(0,LOW);
  pinMode(2,INPUT);  digitalWrite(2,LOW);
  pinMode(5,INPUT);
  delay(5000);
  
  // Lilytiny 24.3mA (+3.5mA) / BadUSB 21.7mA (+3.2mA)
  blink(2);
  pinMode(1,OUTPUT); digitalWrite(1,HIGH);
  pinMode(3,INPUT);  digitalWrite(3,LOW);
  pinMode(4,INPUT);  digitalWrite(4,LOW);
  pinMode(0,INPUT);  digitalWrite(0,LOW);
  pinMode(2,INPUT);  digitalWrite(2,LOW);
  pinMode(5,INPUT);
  delay(5000);
  
  // Lilytiny 32.4mA (+11.6mA) / BadUSB 27.3mA (+8.8mA)
  blink(3);
  pinMode(1,OUTPUT); digitalWrite(1,HIGH);
  pinMode(3,OUTPUT); digitalWrite(3,HIGH);
  pinMode(4,INPUT);  digitalWrite(4,LOW);
  pinMode(0,INPUT);  digitalWrite(0,LOW);
  pinMode(2,INPUT);  digitalWrite(2,LOW);
  pinMode(5,INPUT);
  delay(5000);
  
  // Lilytiny 32.5mA (+11.7mA) / BadUSB 33.0mA (+14.5mA)
  blink(4);
  pinMode(1,OUTPUT); digitalWrite(1,HIGH);
  pinMode(3,OUTPUT); digitalWrite(3,HIGH);
  pinMode(4,OUTPUT); digitalWrite(4,HIGH);
  pinMode(0,INPUT);  digitalWrite(0,LOW);
  pinMode(2,INPUT);  digitalWrite(2,LOW);
  pinMode(5,INPUT);
  delay(5000);

  // Lilytiny 38.8mA (+18.0mA) / BadUSB 32.9mA (+14.4mA)
  blink(5);
  pinMode(1,OUTPUT); digitalWrite(1,HIGH);
  pinMode(3,OUTPUT); digitalWrite(3,HIGH);
  pinMode(4,OUTPUT); digitalWrite(4,HIGH);
  pinMode(0,INPUT_PULLUP); 
  pinMode(2,INPUT_PULLUP); 
  pinMode(5,INPUT_PULLUP); 
  delay(5000);  

}
