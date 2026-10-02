

#include <Arduino.h>
#include <NewPing.h>
#include <TFT_eSPI.h>

#define ENA 17
#define IN1 1
#define IN2 2
#define IN3 21
#define IN4 10
#define ENB 18

#define ECHO_L 11
#define TRIG_L 12
#define ECHO_R 13
#define TRIG_R 16
#define MAX_DISTANCE 200 // Most accurate

#define LCD_POWER 15

#define LINE_L 43 // High "1" as int on BLACK Low "0" as int on WHITE
#define LINE_R 44

bool leftButton = false;
bool rightButton = false;

float Kp = 5.0;

TFT_eSPI tft = TFT_eSPI();

NewPing sonarL(TRIG_L, ECHO_L, MAX_DISTANCE);
NewPing sonarR(TRIG_R, ECHO_R, MAX_DISTANCE);

int leftEdgeDetected() {
  int leftLineStatus = digitalRead(LINE_L); // digital interface will be assigned a value of 3 to read val
  return leftLineStatus;
}
  
int rightEdgeDetected() {
  int rightLineStatus = digitalRead(LINE_R); // digital interface will be assigned a value of 3 to read val
  return rightLineStatus;
}

void setup() {
  Serial.begin(115200);
  
  pinMode (LINE_L, INPUT); // define tracing sensor L output interface
  pinMode (LINE_R, INPUT); // define tracing sensor R output interface

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10,10);
  tft.setTextSize(2);
  tft.setRotation(1);
}

// 1. Line sensors
// 2. Detect opponent
// 3. Sumobot behaviour

void loop() {
  
  if (leftEdgeDetected() || rightEdgeDetected()) {
    if (leftEdgeDetected() && rightEdgeDetected()) {
       
    }

    else if (leftEdgeDetected()) {
      
    }

    else if (rightEdgeDetected()) {

    }
  }
}