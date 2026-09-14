#include <Arduino.h>
#include <NewPing.h>

#define LEFT_BUTTON 0
#define RIGHT_BUTTON 14

#define ENA 17
#define IN1 1
#define IN2 2
#define IN3 21
#define IN4 10
#define ENB 18
#define TRIG1 12
#define ECHO1 11
#define TRIG2 16
#define ECHO2 13

bool leftButton = false;
bool rightButton = false;

void setup() {
  Serial.begin(115200);
}

void loop() {
  bool leftButton = !digitalRead(LEFT_BUTTON);
  bool rightButton = !digitalRead(RIGHT_BUTTON);
}