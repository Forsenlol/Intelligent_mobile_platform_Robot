int pin1 = 0;
int pin2 = 16;
int pin3 = 5;
int pin4 = 4;

int pins1 = 14;
int pins2 = 12;

int pind1 = 13;
int pind2 = 15;

void setup() {

  //PinMode
  pinMode(pin1,OUTPUT);
  pinMode(pin2,OUTPUT);
  pinMode(pin3,OUTPUT);
  pinMode(pin4,OUTPUT);

  pinMode(pins1,OUTPUT);
  pinMode(pins2,OUTPUT);

  digitalWrite(pin1,HIGH);
  digitalWrite(pin2,LOW);
  digitalWrite(pin3,HIGH);
  digitalWrite(pin4,LOW);
  
  analogWrite(pins1,230);
  analogWrite(pins2,230);
        
}

void loop() {
  // put your main code here, to run repeatedly:

}
