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

enum DataAktif {
  None,
  AktifX,
  AktifY,
  AktifZ
} dataAktif;
String kordinat;
int xToGo=0, yToGo=0, zToGo=0;
bool isJogActive = false;

void loop() {
  while(Serial.available() > 0){
    char dataIn = Serial.read();

    switch(dataIn){
      case 'X':
        dataAktif = AktifX;
      break;
      case 'Y':
        dataAktif = AktifY;
      break;
      case 'Z':
        dataAktif = AktifZ;
      break;
      default:
        if(isDigit(dataIn)){
          kordinat += dataIn;
        } else if(dataIn == ' ' || dataIn == '\n') {
          switch(dataAktif){
            case AktifX:
            xToGo = kordinat.toInt();
            xToGo -= X_AXIS.getPositionInCm();
            kordinat = "";
            break;
            case AktifY:
            yToGo = kordinat.toInt();
            yToGo -= Y_AXIS.getPositionInCm();
            kordinat = "";
            break;
            case AktifZ:
            zToGo = kordinat.toInt();
            zToGo -= Z_AXIS.getPositionInCm();
            kordinat = "";
            break;
            default:
            kordinat = "";
            break;
          }
          if(dataIn == '\n') isJogActive = true;
        }
      break;
    }
  }
  Serial.println((String) xToGo + '\t' + yToGo + '\t' + zToGo);
  Serial.println((String) X_AXIS.getPosition() + '\t' + X_AXIS.getPositionInCm());
  delay(100);

  if(isJogActive){
    if(xToGo != 0 && yToGo != 0){
      X_AXIS.cmSync(Y_AXIS, xToGo, yToGo);
    } else if(xToGo != 0 && yToGo == 0){
      X_AXIS.cm(xToGo);
    } else if(xToGo == 0 && yToGo != 0){
      Y_AXIS.cm(yToGo);
    }
    if(zToGo != 0){
      Z_AXIS.cm(zToGo);
    }
    isJogActive = false;
  }
  
//  Y_AXIS.step(32000);
//  X_AXIS.stepSync(Y_AXIS, 3200, 32000);
//  delay(1000);
//  X_AXIS.stepSync(Y_AXIS, 32000, 3200);
//  X_AXIS.step(3200);
//  delay(1000);
//  Serial.print(digitalRead(X_LS_MIN));
//  Serial.print('\t');
//  Serial.print(digitalRead(X_LS_MAX));
//  Serial.print('\t');
//  Serial.print(digitalRead(E0_LS_MIN));
//  Serial.print('\t');
//  Serial.print(digitalRead(E0_LS_MAX));
//  Serial.print('\t');
//  Serial.print(digitalRead(Y_LS_MIN));
//  Serial.print('\t');
//  Serial.print(digitalRead(Y_LS_MAX));
//  Serial.print('\t');
//  Serial.print(digitalRead(Z_LS_MIN));
//  Serial.print('\t');
//  Serial.print(digitalRead(Z_LS_MAX));
//  Serial.print('\n');
}
