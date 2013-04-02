#include <Servo.h> 

// digital pins
#define trigPin 3
#define echoPin 2
#define buttonPin 5
#define leftDrivePin 6 
#define rightDrivePin 7

// analog pins
#define leftCorrPin 4
#define rightCorrPin 5

#define PROG_TURN 0
#define PROG_STOP 1
#define PROG_GO 1

#define turnSpeed 10

Servo leftDrive;
Servo rightDrive;
int leftCorr;
int rightCorr;

boolean configMode = true;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

int programm = 0;

void setup() { 
  Serial.begin(9600);

  // setup button
  pinMode(buttonPin, INPUT);
  
  // setup sonar
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // setup drive  
  leftDrive.attach(leftDrivePin);
  rightDrive.attach(rightDrivePin);

  Serial.print("Switching to mode:");
  Serial.println(configMode ? "CONFIG" : "MEEPMEEP");
} 
 
void loop() { 
  
  if (configMode) {
    leftCorr = analogRead(leftCorrPin);
    rightCorr = analogRead(rightCorrPin);
    
    leftCorr = map(leftCorr, 0, 1023, -10, 10);
    rightCorr = map(rightCorr, 0, 1023, -10, 10);
    
    leftDrive.write(90+leftCorr);
    rightDrive.write(90+rightCorr);
    
    
  if (readButton()) {
    configMode = !configMode;
    Serial.print("Switching to mode:");
    Serial.println(configMode ? "CONFIG" : "MEEPMEEP");
  }
    delay(50);

  } else {
    // meep meep
    int dist = getDistance();
    if (dist == -1 || dist > 50) {
      go();
    } else {
      turn();
    }
    delay(20);
  }
} 

void turn() {
        if (programm != PROG_TURN) {
          Serial.println("turn..");
          leftDrive.write(90+leftCorr+turnSpeed);
          rightDrive.write(90+rightCorr+turnSpeed);  
          programm = PROG_TURN;
        }
}

void go() {
        if (programm != PROG_GO) {
          Serial.println("go..");
          leftDrive.write(0);
          rightDrive.write(179);      
          programm = PROG_GO;
        }
}

void stop() {
        if (programm != PROG_STOP) {
          Serial.println("stop..");
        leftDrive.write(90+leftCorr);
        rightDrive.write(90+rightCorr);
        programm = PROG_STOP; 
        }
}

int getDistance() {
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance >= 200 || distance <= 0){
    Serial.println(distance);
    Serial.println("Out of range");
    return -1;
  }
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}

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
