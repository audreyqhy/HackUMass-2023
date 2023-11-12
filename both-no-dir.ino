#include <Stepper.h>
#include <Smoothed.h>
//#include <math.h>

#define RIGHT 1
#define LEFT 0

#define STOP 0
#define GO 1

const int stepsPerRevolution = 2038;
const int trigPinR = 6;
const int echoPinR = 7;
const int trigPinL = 4;
const int echoPinL = 5;
unsigned int cmR, cmL, temp_L, temp_R;

Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

short int direction = RIGHT;
short int goFlag = STOP;
// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence

short int position = 0;
Smoothed<float> distL;
Smoothed<float> distR;


void setup() {
  Serial.begin(9600);
  pinMode(0,OUTPUT);
  pinMode(1,OUTPUT);
  pinMode(trigPinR, OUTPUT);
  pinMode(trigPinL, OUTPUT);
  pinMode(echoPinR, INPUT);
  pinMode(echoPinL, INPUT);
  distR.begin(SMOOTHED_AVERAGE, 3);
  distL.begin(SMOOTHED_AVERAGE, 3);
  myStepper.setSpeed(2);
}

void measureDistR() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPinR, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinR, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.

  unsigned long duration = pulseIn(echoPinR, HIGH, 10000);
  // Convert the time into a distance
  if (duration == 0) duration = 10000;
  temp_R = duration * 0.5 * 0.03;  // Divide by 29.1 or multiply by 0.0343
                                   /*if (temp_R >= 100) {
    distR.add(100);
  } else distR.add(temp_R);
  */
  distR.add(temp_R);

  cmR = (unsigned int)distR.get();
  Serial.print("cmR:");
  Serial.print(cmR);
  //Serial.println();
  //lastDistR = temp_cm;
}
void measureDistL() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPinL, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinL, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPinL, INPUT);
  unsigned long duration = pulseIn(echoPinL, HIGH, 10000);
  if (duration == 0) duration = 10000;

  temp_L = duration * 0.5 * 0.03;  // Divide by 29.1 or multiply by 0.0343
  /*if (temp_L >= 100) {
    distL.add(100);
  } else*/
  distL.add(temp_L);

  cmL = (unsigned int)distL.get();
  Serial.print(",cmL:");
  Serial.print(cmL);
  //Serial.println();
  //lastDistL = temp_cm;
}

void right60Back() {
  myStepper.step(stepsPerRevolution / 6);
  delay(100);
  myStepper.step(-stepsPerRevolution / 6);
}

void left60Back() {
  myStepper.step(-stepsPerRevolution / 6);
  delay(100);

  myStepper.step(stepsPerRevolution / 6);
}

void left90() {
}

void right() {
  //if (position >= 3) return;
  myStepper.step(50);
  position++;
}

void left() {
  //if (position <= -3) return;
  myStepper.step(-50);
  position--;
}

void move1() {
  if (!goFlag) {
    Serial.println("STOP");
    return;
  }
  if (RIGHT == direction) {
    direction <= 5 ? right() : left();
  } else {
    direction >= -5 ? left() : right();
  }
}

void loop1() {
  
  //delay(100);
  Serial.println("moving");
  left60Back();
  Serial.println("done");
  
  measureDistR();
  delay(1);
  measureDistL();
  Serial.println();
  goFlag = STOP;

  if (abs(cmR - cmL) >= 36) {
    goFlag = GO;
  } else {
    delay(3000);
  }
  if (cmL - cmR >= 80) {
    direction = LEFT;
  } else if (cmR - cmL >= 80) {
    direction = RIGHT;
  }
  //move();
}

void move() {
  if (!goFlag) {
    Serial.println("STOP");
    delay(3000);
    return;
  }
  if (RIGHT == direction) {
    right();
    if (position>=5) direction = LEFT;
  } else {
    left();
    if (position<=-5) direction = RIGHT;
  }
}

unsigned int abs1(int num) {
  return num<0?num*-1:num;
}

void loop() {
  measureDistR();
  measureDistL();

  unsigned int diff = abs1(cmR-cmL);
  Serial.print(",diff:");
  Serial.print(diff);
  Serial.println();
  if (diff<=28 && diff != 0) {
    goFlag = STOP;
  } else {
    goFlag = GO;
  }
  move();
}