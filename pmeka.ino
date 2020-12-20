#include "Stepper.h"

/*
 * PIN MAPPING
 */
#define X_STEP A0
#define X_DIR A1
#define X_EN 38
#define X_LS_MIN 3
#define X_LS_MAX 2

#define Y_STEP A6
#define Y_DIR A7
#define Y_EN A2
#define Y_LS_MIN 14
#define Y_LS_MAX 15

#define Z_STEP 46
#define Z_DIR 48
#define Z_EN A8
#define Z_LS_MIN 18
#define Z_LS_MAX 19

#define E0_STEP 26
#define E0_DIR 28
#define E0_EN 24
#define E0_LS_MIN 20
#define E0_LS_MAX 21

#define E1_STEP 36
#define E1_DIR 34
#define E1_EN 30
#define E1_LS_MIN 0
#define E1_LS_MAX 0
/*
 * Configuration
 */
#define NUM_STEP 3200     // perlu berapa step untuk 1 rotasi
#define CM_STEP 3         // Perlu berapa rotasi untuk 1 CM
#define MIN_DELAY_SPEED 10   // kecepatan default motor

Stepper X_AXIS(X_STEP, X_DIR, X_EN, X_LS_MIN, X_LS_MAX);
Stepper X_AXIS_2(E0_STEP, E0_DIR, E0_EN, E1_LS_MIN, E1_LS_MAX);
Stepper Y_AXIS(Y_STEP, Y_DIR, Y_EN, Y_LS_MIN, Y_LS_MAX);
Stepper Z_AXIS(Z_STEP, Z_DIR, Z_EN, Z_LS_MIN, Z_LS_MAX);

void setup() {
  Serial.begin(115200);
  X_AXIS.begin();
  X_AXIS_2.begin();
  Y_AXIS.begin();
  Z_AXIS.begin();

  X_AXIS.join(X_AXIS_2);
  Y_AXIS.join(Z_AXIS);
  delay(2000);
}

void loop() {
//  Y_AXIS.step(32000);
//  X_AXIS.stepSync(Y_AXIS, 3200, 32000);
//  delay(1000);
//  X_AXIS.stepSync(Y_AXIS, 32000, 3200);
//  X_AXIS.step(3200);
//  delay(1000);
}
