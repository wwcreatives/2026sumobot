#include <Arduino.h>
#include <NewPing.h>
#include <TFT_eSPI.h>

// Right: IN1/2 Left: IN3/4
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

// PWM Assignments
#define PWM_CH_L 0
#define PWM_CH_R 1

#define PWM_FREQ 1000
#define PWM_RES 12
#define MAX_PWM 4095

#define TURN_90_TIME  420
#define TURN_180_TIME 840
#define TURN_360_TIME 1680

#define OPPONENT_THRESHOLD 60
#define ATTACK_DISTANCE 30

bool leftButton = false;
bool rightButton = false;

float Kp = 50.0;

TFT_eSPI tft = TFT_eSPI();

NewPing sonarL(TRIG_L, ECHO_L, MAX_DISTANCE);
NewPing sonarR(TRIG_R, ECHO_R, MAX_DISTANCE);

int getPWM(int pwmPercent) {
  return (pwmPercent * MAX_PWM) / 100; // Max 100 percent PWM (half speed 50%, etc.)
}

int leftEdgeDetected() {
  int leftLineStatus = !digitalRead(LINE_L); // digital interface will be assigned a value of 3 to read val
  return leftLineStatus;
}
int rightEdgeDetected() {
  int rightLineStatus = !digitalRead(LINE_R); // digital interface will be assigned a value of 3 to read val
  return rightLineStatus;
}

int leftSensorDistance() {
  return sonarL.ping_cm();
}
int rightSensorDistance() {
  return sonarR.ping_cm();
}

void moveForward(int leftPWM, int rightPWM) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(PWM_CH_L, leftPWM);
  ledcWrite(PWM_CH_R, rightPWM);
}
void moveBackward(int leftPWM, int rightPWM) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(PWM_CH_L, leftPWM);
  ledcWrite(PWM_CH_R, rightPWM);
}
void dontMove() {
  ledcWrite(PWM_CH_L, 0);
  ledcWrite(PWM_CH_R, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Functions for left right pivoting/turning on spot NOT for driving
void turnLeft(int leftPWM, int rightPWM) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(PWM_CH_L, leftPWM);
  ledcWrite(PWM_CH_R, rightPWM);
}
void turnRight(int leftPWM, int rightPWM) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(PWM_CH_L, leftPWM);
  ledcWrite(PWM_CH_R, rightPWM);
}

void setup() {
  Serial.begin(115200);
  
  pinMode (LINE_L, INPUT); // define tracing sensor L output interface
  pinMode (LINE_R, INPUT); // define tracing sensor R output interface

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(PWM_CH_L, PWM_FREQ, PWM_RES);
  ledcSetup(PWM_CH_R, PWM_FREQ, PWM_RES);

  ledcAttachPin(ENA, PWM_CH_R);
  ledcAttachPin(ENB, PWM_CH_L);

  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(10,10);
  tft.setTextSize(2);
  tft.setRotation(1);
}

void loop() {
  // if (leftEdgeDetected() || rightEdgeDetected()) {
  //   if (leftEdgeDetected() && rightEdgeDetected()) {
  //   }
  //   else if (leftEdgeDetected()) {
  //   }
  //   else if (rightEdgeDetected()) {
  //   }
  //   return;
  // }

  int leftDistance = leftSensorDistance();
  int rightDistance = rightSensorDistance();

  bool leftEnemyDetected = leftDistance > 0 && leftDistance <= OPPONENT_THRESHOLD;
  bool rightEnemyDetected = rightDistance > 0 && rightDistance <= OPPONENT_THRESHOLD;

  if (leftEnemyDetected && rightEnemyDetected) {
    tft.setCursor(10,10);
    tft.print("ENEMY DETECTED");

    int error = leftDistance - rightDistance;
    int correction = Kp * error;

    int basePWM = getPWM(70);

    // If opponent is at attack distance and centred, attack at full speed
    if (leftDistance <= ATTACK_DISTANCE && rightDistance <= ATTACK_DISTANCE && abs(error) <= 3) {
      moveForward(MAX_PWM, MAX_PWM);
    }

    else {
      int leftPWM = constrain(basePWM + correction, 0, MAX_PWM);
      int rightPWM = constrain(basePWM - correction, 0, MAX_PWM);
      moveForward(leftPWM, rightPWM);
    }
  }

  // ONLY LEFT ultrasonic detects opponent
  else if (leftEnemyDetected) {
    tft.setCursor(10,10);
    tft.print("ENEMY DETECTED ON LEFT");

    turnLeft(getPWM(55), getPWM(55));
  }

  // ONLY RIGHT ultrasonic detects opponent
  else if (rightEnemyDetected) {
    tft.setCursor(10,10);
    tft.print("ENEMY DETECTED ON RIGHT");
    turnRight(getPWM(55), getPWM(55));
  }

  // NEITHER ultrasonic detects opponent
  else {
    tft.setCursor(10,10);
    tft.print("ENEMY NOT FOUND");
    // Search for opponent by continuously rotating
    turnRight(getPWM(45), getPWM(45));
  }
  tft.fillRect(10,10,50,30,TFT_BLACK);
}