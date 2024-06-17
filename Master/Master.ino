#include <Wire.h>

#define encxa 2
#define encxb 5
#define encya 3
#define encyb 6
#define startpuntx 12
#define startpunty 7
#define buttonPin 4

int joystickButton = 10;
unsigned long delay1 = 0;
int pwrPinFork = 11;
int directionPinFork = 13;
bool laatsteknopstatus = false;
int heenEnWeer = 0;
int posx = 0;
int posy = 0;
int targetx = 999999999;
int targety = 999999999;
int tolerancex = 0;
int tolerancey = 0;

unsigned long previousMillis = 0;
unsigned long positiedoorstuur = 0;
const long interval = 1500; // interval at which to stop the motor (milliseconds)
bool motorRunning = false;
bool motorDirection = LOW; // false for LOW, true for HIGH
bool buttonState = false;
bool lastButtonState = false;
bool buttonToggle = false;
bool locatieBesturen = false;
bool rechts = false;
bool links = false;
bool omhoog = false;
bool omlaag = false;
bool yOpvangen = false;
bool veranderBesturing = false;

String HMIdoorstuur;

void setup() {
  positiedoorstuur = millis();
  pinMode(startpuntx, INPUT);
  pinMode(startpunty, INPUT);
  pinMode(encxa, INPUT);
  pinMode(encxb, INPUT);
  pinMode(encya, INPUT);
  pinMode(encyb, INPUT);
  pinMode(joystickButton, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(pwrPinFork, OUTPUT);
  pinMode(directionPinFork, OUTPUT);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encxa), readEncoderx, RISING);
  attachInterrupt(digitalPinToInterrupt(encya), readEncodery, RISING);
  TCCR2B = TCCR2B & B11111000 | B00000111; // for PWM frequency of 30 Hz 
  delay1 = millis();
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState && buttonState == LOW) {
    analogWrite(pwrPinFork, 0);
    stuurbericht("stil");
    buttonToggle = !buttonToggle;
  }
  lastButtonState = buttonState;

  if (buttonToggle) {
    if (digitalRead(startpuntx)) {
      posx = 0;
    }
    if(digitalRead(startpunty)){
      posy = 0;
    }

    if (Serial.available() > 0) {
      HMIdoorstuur = Serial.readStringUntil('\n');
      if (HMIdoorstuur == "fork") {
        knopingedrukt();
      } 
      else if (HMIdoorstuur.startsWith("coordinaten")) {
        ontvangenCoordinates(HMIdoorstuur);
      } 
      else {
        stuurbericht(HMIdoorstuur);
      }
    }
    
    if(locatieBesturen == true){
      naarLocatie();
    }

    
      // if((posx > (targetx - tolerancex)) && (posx < (targetx + tolerancex))){
      //   stuurbericht("right");
      // }
      //   if((posx > (targety - tolerancey)) && (posx < (targety + tolerancey))){
      //   stuurbericht("up");
      // }


    //while ((millis() - positiedoorstuur) > 250) {
      String Coordinaten = "COORD," + String(posx) + "," + String(posy);
      Serial.println(Coordinaten);
      positiedoorstuur = millis();
    //}

    eenmaalknopindrukken();
    checkMotor();
    falloverstate(); // Check if the fork is in the "fall over" state
  }
}

void eenmaalknopindrukken() {
  bool knop1 = digitalRead(joystickButton);
  while ((millis() - delay1) > 10){
    delay1 = millis();
  if (knop1 != laatsteknopstatus && knop1 == HIGH) {    // ensures that the button press signal is only sent once
    knopingedrukt();
  }
  laatsteknopstatus = knop1;
  }
}

void knopingedrukt() {
  heenEnWeer++;
  if (heenEnWeer == 1) {
    moveForward();
  } else if (heenEnWeer == 2) {
    moveBackward();
    heenEnWeer = 0;
  }
}

void moveForward() {
  motorDirection = LOW;
  startMotor();
}

void moveBackward() {
  motorDirection = HIGH;
  startMotor();
}

void startMotor() {
  digitalWrite(directionPinFork, motorDirection);
  analogWrite(pwrPinFork, 255);
  previousMillis = millis();
  motorRunning = true;
}

void checkMotor() {
  if (motorRunning) {
    if (digitalRead(A3) == LOW) {
      // Microswitch is pressed, continue motor operation
      if (millis() - previousMillis >= interval) {
        analogWrite(pwrPinFork, 0);
        motorRunning = false;
      }
    } else {
      // Microswitch is not pressed, stop the motor immediately
      analogWrite(pwrPinFork, 0);
      motorRunning = false;
    }
  }
}

void falloverstate() {
  int switchState = digitalRead(A3);
  if (switchState == HIGH) {
    bool knop1 = digitalRead(joystickButton);
    // If switchState is HIGH, move the fork backwards by clicking the joystick button
    if (knop1 == LOW) {
      moveBackward();
    }
    // Reset heenEnWeer to ensure proper operation of the fork movement
    heenEnWeer = 0;
    // Wait for a short moment to allow the fork to move
    // Simulate a button press to avoid continuous movement
  }
}

void buttonToggleState() {
  bool buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState && buttonState == LOW) {
    buttonToggle = !buttonToggle;
    stuurbericht("stil");
  }
  lastButtonState = buttonState;
}

void readEncodery() {
  int c = digitalRead(encyb);
  if (c > 0) {
    posy--;
  } else {
    posy++;
  }
}

void readEncoderx() {
  int b = digitalRead(encxb);
  if (b > 0) {
    posx++;
  } else {
    posx--;
  }
}

void ontvangenCoordinates(String input) {
  int commaIndex1 = input.indexOf(',');
  int commaIndex2 = input.indexOf(',', commaIndex1 + 1);
  if (commaIndex1 != -1 && commaIndex2 != -1) {
    int x = input.substring(commaIndex1 + 1, commaIndex2).toInt();
    int y = input.substring(commaIndex2 + 1).toInt();
    gaNaarLocatieStart(x,y);
    } 
  }

void gaNaarLocatieStart(int target_x, int target_y) {
      targetx = target_x;
      targety = target_y;
      locatieBesturen = true;

      if(targetx > posx){
        rechts = true;
        for (int i = 0; i < 3; i++) {
        stuurbericht("right"); 
        }
      } else if(targetx < posx){
        links = true;
        for (int i = 0; i < 3; i++) {
        stuurbericht("left");   
        }
      } else if(targetx == posx){
        veranderBesturing = true;
      }
}

void naarLocatie() {
  if ((posx > (targetx - 10)) && (posx < (targetx + 10)) && (rechts == true)) {
    stuurbericht("right");
    rechts = false;
    veranderBesturing = true;
  } else if ((posx > (targetx - 10)) && (posx < (targetx + 10)) && (links == true)) {
    stuurbericht("left");
    links = false;
    veranderBesturing = true;
  }
 
  if(veranderBesturing){
    veranderBesturing = false;
    if(!omhoog || !omlaag){
      if(targety > posy){
        omhoog = true;
        yOpvangen = true;
        stuurbericht("up");
      } else if(targety < posy){
        omlaag = true;
        yOpvangen = true;
        stuurbericht("down");
      }
    }
  }

  if(yOpvangen){
    if ((posy > (targety - 20)) && (posy < (targety + 20)) && (omhoog == true)) {
      stuurbericht("up");
      omhoog = false;
      yOpvangen = false;
      locatieBesturen = false;

    } else if ((posy > (targety - 20)) && (posy < (targety + 20)) && (omlaag == true)) {
      stuurbericht("down");
      stuurbericht("up");
      delay(20);
      stuurbericht("up");
      omlaag = false;
      yOpvangen = false;
      locatieBesturen = false;

    }
  }
}

void stuurbericht(String bericht) {
  Wire.beginTransmission(4);
  Wire.write(bericht.c_str());
  Wire.endTransmission();
}