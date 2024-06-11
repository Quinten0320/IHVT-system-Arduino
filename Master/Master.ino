#include <Wire.h>

#define encxa 2
#define encxb 5
#define encya 3
#define encyb 6
#define startpuntx 8
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
unsigned long previousMillis = 0;
unsigned long positiedoorstuur = 0;
const long interval = 1500; // interval at which to stop the motor (milliseconds)
bool motorRunning = false;
bool motorDirection = LOW; // false for LOW, true for HIGH
bool buttonState = false;
bool lastButtonState = false;
bool buttonToggle = false;
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
      } else {
        stuurbericht(HMIdoorstuur);
      }
    }

    while ((millis() - positiedoorstuur) > 250) {
      String Coordinaten = "COORD," + String(posx) + "," + String(posy);
      Serial.println(Coordinaten);
      positiedoorstuur = millis();
    }

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
    posy++;
  } else {
    posy--;
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

void stuurbericht(String bericht) {
  Wire.beginTransmission(4);
  Wire.write(bericht.c_str());
  Wire.endTransmission();
}