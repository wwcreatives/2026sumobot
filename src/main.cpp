// This program is built for the first progress mark of ELEC3020.

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

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
}


void dontMove() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

bool leftButton = false;
bool rightButton = false;

TFT_eSPI tft = TFT_eSPI();
// NewPing sonarL(TRIG1, ECHO1, MAX_DISTANCE);
// NewPing sonarR(TRIG2, ECHO2, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LCD_POWER_ON, OUTPUT);
  digitalWrite(LCD_POWER_ON, HIGH);
  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);

  dontMove();
  
  pinMode (LINE_L, INPUT); // define tracing sensor L output interface
  pinMode (LINE_R, INPUT); // define tracing sensor R output interface

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);
  tft.setCursor(10,10);
  tft.setTextSize(2);
  delay(1000);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("%-10s", "Forward");
  moveForward();
  delay(2000); // Change for our own car when 1m how long delay

  dontMove();
  tft.setCursor(10, 10);
  tft.printf("%-10s", "Stop");
  delay(2000);

  tft.setCursor(10, 10);
  tft.printf("%-10s", "Backward");
  moveBackward();
  delay(2000);// Again change

  dontMove();
  tft.setCursor(10, 10);
  tft.printf("%-10s", "Finished");
}

void loop() {

}