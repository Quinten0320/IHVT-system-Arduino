#include <Wire.h>

int directionPinX = 12;
int powerPinX = 3;
int directionPinY = 13;
int powerPinY = 11;
int forkButton = 10;
#define VRX_PIN  A2
#define VRY_PIN  A3
#define MICRO_SWITCH_PIN_RIGHT 2
#define MICRO_SWITCH_PIN_LEFT 4
#define MICRO_SWITCH_PIN_UP 7
#define MICRO_SWITCH_PIN_DOWN 10
#define startpuntx 5
#define startpunty 6

int xValue = 0;
int yValue = 0;
bool buttonStateRight = false;
bool buttonStateLeft = false;
bool buttonStateUp = false;
bool buttonStateDown = false;
bool HMIBesturing = false;
String HMIstuur;
String laatstestring = "";

void omhoog();
void omlaag();
void links();
void rechts();
void joyStick();
void bewegen();
void HMIBewegen();
void receiveEvent(int howMany);

void setup() {
  pinMode(directionPinX, OUTPUT);
  pinMode(powerPinX, OUTPUT);
  pinMode(powerPinY, OUTPUT);
  pinMode(directionPinY, OUTPUT);
  pinMode(startpuntx, OUTPUT);
  pinMode(startpunty, OUTPUT);
  pinMode(forkButton, INPUT_PULLUP);
  pinMode(MICRO_SWITCH_PIN_RIGHT, INPUT_PULLUP);
  pinMode(MICRO_SWITCH_PIN_LEFT, INPUT_PULLUP); 
  pinMode(MICRO_SWITCH_PIN_UP, INPUT_PULLUP); 
  pinMode(MICRO_SWITCH_PIN_DOWN, INPUT_PULLUP); 
  Serial.begin(9600);
  Wire.begin(4);
  Wire.onReceive(receiveEvent);

  TCCR2B = TCCR2B & B11111000 | B00000111; // for PWM frequency of 30 Hz 
}

void loop() {
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
  digitalWrite(startpuntx, LOW);
  digitalWrite(startpunty, LOW);

  buttonStateRight = !digitalRead(MICRO_SWITCH_PIN_RIGHT); 
  buttonStateLeft = !digitalRead(MICRO_SWITCH_PIN_LEFT); 
  buttonStateUp = !digitalRead(MICRO_SWITCH_PIN_UP); 
  buttonStateDown = digitalRead(MICRO_SWITCH_PIN_DOWN);

  if(!HMIBesturing){
    joyStick();
  }
  //bewegen();
}

void bewegen() {
  if (HMIBesturing) {
    HMIBewegen();
  } else {
    joyStick();
  }
}

void HMIBewegen(String kant) {
  HMIBesturing = true;
  if(kant == "left"){
    if(laatstestring == "left"){
      analogWrite(powerPinX, 0);
      laatstestring = "";
    }else{
      laatstestring = kant;
      rechts();
    }
  }else if(kant == "right"){
    if(laatstestring == "right"){
      analogWrite(powerPinX, 0);
      laatstestring = "";
    }else{
      laatstestring = kant;
      links();
    }
  }else if(kant == "up"){
    if(laatstestring == "up"){
      analogWrite(powerPinY, 0);
      laatstestring = "";
    }else{
      laatstestring = kant;
      omhoog();
    }
  }else if(kant == "down"){
    if(laatstestring == "down"){
      analogWrite(powerPinY, 0);
      laatstestring = "";
    }else{
      laatstestring = kant;
      omlaag();
    }
  }
}

void joyStick() {
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  if (xValue < 400) {
    links();
  } else if (xValue > 600) {
    rechts();
  } else {
    analogWrite(powerPinX, 0);
  }

  if (yValue > 600) {
    omlaag();
  } else if (yValue < 400) {
    omhoog();
  } else {
    analogWrite(powerPinY, 0);
  }
}

void receiveEvent(int howMany) {
  String receivedString = "";
  while (Wire.available()) {
    char c = Wire.read();
    receivedString += c;
  }
  // HMIstuur = receivedString;
  // HMIBesturing = true;
  HMIBewegen(receivedString);
}

void rechts() {
  if (!buttonStateLeft) {
    digitalWrite(directionPinX, LOW);
    analogWrite(powerPinX, 255);
  } else {
    HMIBesturing = false;
    digitalWrite(startpuntx, HIGH);
    analogWrite(powerPinX, 0);
  }
}

void links() {
  if (!buttonStateRight) {
    digitalWrite(directionPinX, HIGH);
    analogWrite(powerPinX, 255);
  } else {
    HMIBesturing = false;
    digitalWrite(directionPinX, HIGH);
    analogWrite(powerPinX, 0);
  }
}

void omhoog() {
  if (!buttonStateUp) {
    digitalWrite(directionPinY, LOW);
    analogWrite(powerPinY, 255);
  } else {
    HMIBesturing = false;
    analogWrite(powerPinY, 0);
  }
}

void omlaag() {
  if (!buttonStateDown) {
    digitalWrite(directionPinY, HIGH);
    analogWrite(powerPinY, 255);
  } else {
    digitalWrite(startpunty, HIGH);
    HMIBesturing = false;
    analogWrite(powerPinY, 0);
  }
}
