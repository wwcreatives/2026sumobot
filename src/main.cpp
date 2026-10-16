
#include <Arduino.h>
#include <NewPing.h>
#include <TFT_eSPI.h>
#include "esp_system.h"

// Right: IN1/2 Left: IN3/4
#define ENA 17
#define IN1 1
#define IN2 2
#define IN3 21
#define IN4 10
#define ENB 18

// Only LEFT ultrasonic sensor
#define ECHO_L 11
#define TRIG_L 12
#define MAX_DISTANCE 200

#define LED_PIN 16

#define LCD_POWER 15

// HIGH = BLACK, LOW = WHITE
#define LINE_L 43
#define LINE_R 44

// PWM Assignments
#define PWM_CH_L 0
#define PWM_CH_R 1

#define PWM_FREQ 1000
#define PWM_RES 12
#define MAX_PWM 4095

#define OPPONENT_THRESHOLD 90
#define ATTACK_DISTANCE 70

#define RAMP_INTERVAL 5
#define RAMP_STEP 200

#define TURN_180_TIME 840  // Adjust to your robot's actual 180-degree turn

bool edgeTurning = false;
unsigned long edgeTurnStart = 0;
int edgeTurnDirection = 1;

int currentLeftPWM = 0;
int currentRightPWM = 0;

int targetLeftPWM = 0;
int targetRightPWM = 0;

int currentLeftDirection = 0;
int currentRightDirection = 0;

int targetLeftDirection = 0;
int targetRightDirection = 0;

unsigned long lastRampTime = 0;
unsigned long lastDisplayTime = 0;

TFT_eSPI tft = TFT_eSPI();

NewPing sonarL(TRIG_L, ECHO_L, MAX_DISTANCE);

int getPWM(int pwmPercent) {
  return (pwmPercent * MAX_PWM) / 100;
}

int leftEdgeDetected() {
  return digitalRead(LINE_L);
}

int rightEdgeDetected() {
  return digitalRead(LINE_R);
}

int leftSensorDistance() {
  return sonarL.ping_cm();
}

// Set desired motor direction and PWM
void setMotorTargets(int leftDir, int rightDir,
                     int leftPWM, int rightPWM) {
  targetLeftDirection = leftDir;
  targetRightDirection = rightDir;

  targetLeftPWM = constrain(leftPWM, 0, MAX_PWM);
  targetRightPWM = constrain(rightPWM, 0, MAX_PWM);
}

void moveForward(int leftPWM, int rightPWM) {
  setMotorTargets(1, 1, leftPWM, rightPWM);
}

void moveBackward(int leftPWM, int rightPWM) {
  setMotorTargets(-1, -1, leftPWM, rightPWM);
}

void dontMove() {
  setMotorTargets(0, 0, 0, 0);
}

void turnLeft(int leftPWM, int rightPWM) {
  setMotorTargets(-1, 1, leftPWM, rightPWM);
}

void turnRight(int leftPWM, int rightPWM) {
  setMotorTargets(1, -1, leftPWM, rightPWM);
}

// Gradually change PWM instead of jumping to target speed
void updateMotorRamp() {
  if (millis() - lastRampTime < RAMP_INTERVAL) {
    return;
  }

  lastRampTime = millis();

  // LEFT motor
  if (currentLeftDirection != targetLeftDirection &&
      currentLeftPWM > 0) {
    currentLeftPWM = max(0, currentLeftPWM - RAMP_STEP);
  }
  else {
    currentLeftDirection = targetLeftDirection;

    if (currentLeftPWM < targetLeftPWM) {
      currentLeftPWM = min(targetLeftPWM,
                           currentLeftPWM + RAMP_STEP);
    }
    else if (currentLeftPWM > targetLeftPWM) {
      currentLeftPWM = max(targetLeftPWM,
                           currentLeftPWM - RAMP_STEP);
    }
  }

  // RIGHT motor
  if (currentRightDirection != targetRightDirection &&
      currentRightPWM > 0) {
    currentRightPWM = max(0, currentRightPWM - RAMP_STEP);
  }
  else {
    currentRightDirection = targetRightDirection;

    if (currentRightPWM < targetRightPWM) {
      currentRightPWM = min(targetRightPWM,
                            currentRightPWM + RAMP_STEP);
    }
    else if (currentRightPWM > targetRightPWM) {
      currentRightPWM = max(targetRightPWM,
                            currentRightPWM - RAMP_STEP);
    }
  }

  // RIGHT motor: IN1/IN2
  digitalWrite(IN1, currentRightDirection < 0 ? HIGH : LOW);
  digitalWrite(IN2, currentRightDirection > 0 ? HIGH : LOW);

  // LEFT motor: IN3/IN4
  digitalWrite(IN3, currentLeftDirection < 0 ? HIGH : LOW);
  digitalWrite(IN4, currentLeftDirection > 0 ? HIGH : LOW);

  ledcWrite(PWM_CH_L, currentLeftPWM);
  ledcWrite(PWM_CH_R, currentRightPWM);
}

void setup() {
  Serial.begin(115200);

  pinMode(LINE_L, INPUT);
  pinMode(LINE_R, INPUT);

  pinMode(LCD_POWER, OUTPUT);
  digitalWrite(LCD_POWER, HIGH);

  pinMode(LED_PIN, OUTPUT);

  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ensure motors are stopped at startup
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  ledcSetup(PWM_CH_L, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_R, PWM_FREQ, PWM_RES);

  ledcAttachPin(ENA, PWM_CH_R);
  ledcAttachPin(ENB, PWM_CH_L);

  ledcWrite(PWM_CH_L, 0);
  ledcWrite(PWM_CH_R, 0);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // esp_reset_reason_t reason = esp_reset_reason();

  // tft.setCursor(10, 50);
  // tft.printf("Reset reason: %d", (int)reason);

  // delay(3000);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  int leftLine = leftEdgeDetected();
  int rightLine = rightEdgeDetected();

  const char* status = "ENEMY NOT FOUND";
  int distance = 0;

  // Start a committed 180-degree turn on edge detection
if (!edgeTurning && (leftLine || rightLine)) {
  edgeTurning = true;
  edgeTurnStart = millis();

  // Left or both -> right turn
  // Right only -> left turn
  edgeTurnDirection = (rightLine && !leftLine) ? -1 : 1;
}

// Continue turning even after the line is no longer detected
if (edgeTurning) {
  status = "EDGE TURN 180";

  if (edgeTurnDirection == 1) {
    turnRight(getPWM(100), getPWM(100));
  }
  else {
    turnLeft(getPWM(100), getPWM(100));
  }

  if (millis() - edgeTurnStart >= TURN_180_TIME) {
    edgeTurning = false;
  }
}
else {
    // Read ONLY the left ultrasonic sensor
    distance = leftSensorDistance();

    bool enemyDetected =
        (distance > 0) &&
        (distance <= OPPONENT_THRESHOLD);

    if (enemyDetected) {
      if (distance <= ATTACK_DISTANCE) {
        status = "ATTACKING";
        moveForward(MAX_PWM, MAX_PWM);
        digitalWrite(LED_PIN, HIGH);
      }
      else {
        status = "ENEMY DETECTED";
        moveForward(getPWM(80), getPWM(80));
        digitalWrite(LED_PIN, HIGH);
      }
    }
    else {
      status = "ENEMY NOT FOUND";
      digitalWrite(LED_PIN, LOW);
      // Search by rotating right
      turnRight(getPWM(70), getPWM(70));
    }
  }

  // Apply gradual acceleration/deceleration
  updateMotorRamp();

  // Limit display updates to every 100 ms
  if (millis() - lastDisplayTime >= 100) {
    lastDisplayTime = millis();

    tft.setCursor(10, 10);
    tft.printf("%-23s", status);

    tft.setCursor(10, 40);
    tft.printf("Distance: %3d cm  ", distance);

    tft.setCursor(10, 70);
    tft.printf("L:%4d R:%4d  ",
               currentLeftPWM, currentRightPWM);
  }

  delay(10);
}
