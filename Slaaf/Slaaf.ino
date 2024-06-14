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
  #define startpuntx 8
  #define startpunty 6

  int xValue = 0;
  int yValue = 0;
  bool buttonStateRight = false;
  bool buttonStateLeft = false;
  bool buttonStateUp = false;
  bool buttonStateDown = false;
  bool HMIBesturing = false;
  bool noodStop = true;
  String HMIstuur;
  String laatstestring = "";


  void omhoog();
  void omlaag();
  void links();
  void rechts();
  void HMIomhoog();
  void HMIomlaag();
  void HMIlinks();
  void HMIrechts();
  void joyStick();
  void bewegen();
  void HMIBewegen();
  void noodStopFunctie();
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

    if(buttonStateLeft){
      digitalWrite(startpuntx, HIGH);
    }
    if(buttonStateDown){
      digitalWrite(startpunty, HIGH);
    }

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
    
    if(kant == "left"){
      HMIlinks();
    }else if(kant == "right"){
      HMIrechts();
    }else if(kant == "up"){
      HMIomhoog();
    }else if(kant == "down"){
      HMIomlaag();
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
    if (receivedString == "stil") {
      noodStopFunctie();
    } else {
      HMIBewegen(receivedString);
    }
  }

  void noodStopFunctie(){
    if(!noodStop){
        noodStop = true;
        analogWrite(powerPinX, 0);
        analogWrite(powerPinY, 0);
      } else {
        noodStop = false;
        analogWrite(powerPinX, 255);
        analogWrite(powerPinY, 255);
    }
  }

  void rechts() {
    if(!noodStop){
      if (!buttonStateLeft) {
        digitalWrite(directionPinX, LOW);
        analogWrite(powerPinX, 255);
      } else {
        analogWrite(powerPinX, 0);
      }
    }
  }

  void links() {
    if(!noodStop){
      if (!buttonStateRight) {
        digitalWrite(directionPinX, HIGH);
        analogWrite(powerPinX, 255);
      } else {
        analogWrite(powerPinX, 0);
      }
    }
  }

  void omhoog() {
    if(!noodStop){
      if (!buttonStateUp) {
        digitalWrite(directionPinY, LOW);
        analogWrite(powerPinY, 255);
      } else {
        analogWrite(powerPinY, 0);
      }
    }
  }

  void omlaag() {
    if(!noodStop){
      if (!buttonStateDown) {
        digitalWrite(directionPinY, HIGH);
        analogWrite(powerPinY, 255);
      } else {
        analogWrite(powerPinY, 0);
      }
    }
  }

  void HMIrechts() {
    if(!noodStop){
      if (!buttonStateLeft) {
        if(laatstestring == "right"){
          analogWrite(powerPinX, 0);
          HMIBesturing = false;
          laatstestring = " ";
        }else{
          digitalWrite(directionPinX, HIGH);
          analogWrite(powerPinX, 255);
          laatstestring = "right";
          HMIBesturing = true;
        }
      } else {
        HMIBesturing = false;
        analogWrite(powerPinX, 0);
      }
    }
  }

  void HMIlinks() {
    if(!noodStop){
      if (!buttonStateRight) {
        if(laatstestring == "left"){
          analogWrite(powerPinX, 0);
          HMIBesturing = false;
          laatstestring = " ";
        }else{
          digitalWrite(directionPinX, LOW);
          analogWrite(powerPinX, 255);
          laatstestring = "left";
          HMIBesturing = true;
        }
      } else {
        HMIBesturing = false;
        analogWrite(powerPinX, 0);
      }
    }
  }

  void HMIomhoog() {
    if(!noodStop){
      if (!buttonStateUp) {
        if(laatstestring == "up"){
          analogWrite(powerPinY, 0);
          HMIBesturing = false;
          laatstestring = " ";
        }else{
          digitalWrite(directionPinY, LOW);
          analogWrite(powerPinY, 255);
          laatstestring = "up";
          HMIBesturing = true;
        }
      } else {
        HMIBesturing = false;
        analogWrite(powerPinY, 0);
      }
    }
  }

  void HMIomlaag() {
    if(!noodStop){
      if (!buttonStateDown) {
        if(laatstestring == "down"){
          analogWrite(powerPinY, 0);
          HMIBesturing = false;
          laatstestring = " ";
        }else{
          digitalWrite(directionPinY, HIGH);
          analogWrite(powerPinY, 255);
          laatstestring = "down";
          HMIBesturing = true;
        }
      } else {
        HMIBesturing = false;
        analogWrite(powerPinY, 0);
      }
    }
  }