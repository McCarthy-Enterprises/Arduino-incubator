#include <Servo.h>

Servo myservo;  // create servo object to control a servo

unsigned long delayTime = 7200000; // 2 hour delay
// unsigned long delayTime = 1000 * 5; // 3 second delay

int pos = 0;    // variable to store the servo position

int servoMin = 45;
int servoMax = 135;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void moveSlowly(int from, int to, Servo &servo) { // Pass by reference
  int increment = (from < to) ? 1 : -1; // Determine increment direction
  for (int current = from; current != to; current += increment) {
    servo.write(current);
    delay(50);
  }
}

void loop() {
  moveSlowly(servoMax, servoMin, myservo);
  delay(delayTime); // wait delayTime
  moveSlowly(servoMin, servoMax, myservo);
  delay(delayTime); // wait delayTime
}
