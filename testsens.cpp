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
#define MAX_DISTANCE 200

#define LCD_POWER 15

#define LINE_L 43   // High "1" on BLACK, Low "0" on WHITE
#define LINE_R 44

bool leftButton = false;
bool rightButton = false;

TFT_eSPI tft = TFT_eSPI();

NewPing sonarL(TRIG_L, ECHO_L, MAX_DISTANCE);
NewPing sonarR(TRIG_R, ECHO_R, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);

  pinMode(LINE_L, INPUT);
  pinMode(LINE_R, INPUT);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
}

void loop() {
  int leftLineStatus = digitalRead(LINE_L);
  int rightLineStatus = digitalRead(LINE_R);

  int leftDistance = sonarL.ping_cm();
  int rightDistance = sonarR.ping_cm();

  tft.setCursor(10, 10);
  tft.print("LEFT STAT: ");
  tft.print(leftLineStatus);
  tft.print("   ");

  tft.setCursor(10, 30);
  tft.print("RIGHT STAT: ");
  tft.print(rightLineStatus);
  tft.print("   ");

  tft.setCursor(10, 50);
  tft.print("SONAR L: ");
  tft.print(leftDistance);
  tft.print(" cm   ");

  tft.setCursor(10, 70);
  tft.print("SONAR R: ");
  tft.print(rightDistance);
  tft.print(" cm   ");

  delay(50);
}