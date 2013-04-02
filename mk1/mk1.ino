#include <Servo.h> 

#define MIN_DELAY 2
#define DEBUG

#define PROG_TURN 0
#define PROG_STOP 1
#define PROG_GO 2

#define LEFT 1
#define RIGHT 2

#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x) 
#endif

const float vConv = 5.0/1024.0;

// analog pin setup
const int IRpin = A1;
const int leftCorrPin = A4;
const int rightCorrPin = A5;

// digital pin setup
const int radarPin = 4;
const int buttonPin = 5;
const int leftDrivePin = 6;
const int rightDrivePin = 7;

const int turnSpeed = 10;

// objects
Servo radar;
Servo leftDrive;
Servo rightDrive;

// states
boolean configMode = true;
int programm = PROG_STOP;

// global vars
int leftCorr = 0;
int rightCorr = 0;

// function prototype
float getDistance();
int readButton();

void turn(int dir, int angle);
void go();
void stop();

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  // setup button
  pinMode(buttonPin, INPUT);
  
  // setup drives
  leftDrive.attach(leftDrivePin);
  rightDrive.attach(rightDrivePin);
  
  // setup radar
  radar.attach(radarPin);
  radar.write(95);
}

void loop() {
  if (configMode) {
    leftCorr = analogRead(leftCorrPin);
    rightCorr = analogRead(rightCorrPin);
    
    leftCorr = map(leftCorr, 0, 1023, -10, 10);
    rightCorr = map(rightCorr, 0, 1023, -10, 10);
    
    leftDrive.write(90+leftCorr);
    rightDrive.write(90+rightCorr);
    
    // leave config mode
    if (readButton()) {
      DEBUG_PRINTLN("leave config mode..");
      configMode = !configMode;
    }
  } else {
    // simple main control loop
    int dist = getDistance();
//    DEBUG_PRINTLN(dist);
//    if (dist > 40) {
//      go();
//    } else {
//      turn(LEFT, infinite);
//    }
//    delay(MIN_DELAY);  

    if (dist > 40) {
      go();
    } else {
      int a = getBestAngle();
      int dir = a<0?LEFT:RIGHT;
      turn(dir, abs(a));
      delay(200);
      radar.write(95);
    }
    delay(MIN_DELAY);      
  }
}

int getBestAngle() {
  int dist = 0;
  int angle = 0;
  int distMax = 0;
  for (int a=0;a<180;a+=10) {
    radar.write(a);
    int dist = getDistance();
    if (dist > distMax) {
      angle = a;
      distMax = dist;
    }
    delay(30);      
  }
  return angle-90;
}

float getDistance() {
  return (65*pow(analogRead(IRpin)*vConv, -1.10));
}

int buttonState;
long lastDebounceTime = 0;
long debounceDelay = 50;
int lastButtonState = LOW;
// debouncing button input
int readButton() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  } 
  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState = reading;
  }
  lastButtonState = reading;
  return buttonState;
} 

// drive controls
void turn(int dir, int angle) {
  if (programm != PROG_TURN) {
    if (dir == LEFT) {
      DEBUG_PRINTLN("turn left..");
      leftDrive.write(90+leftCorr+turnSpeed);
      rightDrive.write(90+rightCorr+turnSpeed);  
    } else {
      DEBUG_PRINTLN("turn right..");
      leftDrive.write(90+leftCorr-turnSpeed);
      rightDrive.write(90+rightCorr-turnSpeed);  
    }
    programm = PROG_TURN;
  }
  
  if (angle > 0) {    
    delay(220*(angle/60.0));
    stop();
  }    
}

void go() {
  if (programm != PROG_GO) {
    DEBUG_PRINTLN("go..");
    leftDrive.write(0);
    rightDrive.write(179);      
    programm = PROG_GO;
  }
}

void stop() {
  if (programm != PROG_STOP) {
    DEBUG_PRINTLN("stop..");
    leftDrive.write(90+leftCorr);
    rightDrive.write(90+rightCorr);
    programm = PROG_STOP;
  }
}
