#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

class Stepper {
  public:
  uint8_t stepPin = 0;
  uint8_t dirPin = 0;
  uint8_t enPin = 0;
  uint8_t lsMin = 0;
  uint8_t lsMax = 0;
  
  bool defaultDir = 0;

  int maxCm = 115;
  int step2Cm = 800;
  long pos = 0;
  
  unsigned long delaySpeed = 50;

  Stepper *secondStepper;

  public:
  Stepper(uint8_t sPin, uint8_t dPin, uint8_t ePin, uint8_t lMin, uint8_t lMax); // inisialisasi nomor pinnya
  void begin(); // inisialisasi output dan default statenya
  void disable(); // mematikan stepper
  void enable(); // menyalakan stepper
  void setDelaySpeed(uint16_t dSpeed); // mengatur kecepatan stepper
  void setMaxDist(int cm); // mengganti titik maksimal cm
  int getMaxDist();
  
  void step(long n); // bergerak sesuai step
  void cm(int n); // bergerak sesuai cm
  
  void setStep2Cm(int n); // mengatur 1 cm ada berapa step
  void resetPosition();
  long getPosition();
  int getPositionInCm();
  void home();

  void join(Stepper& s);
  
  void toggleDir(); // ganti direction default secara single
  void setDir(bool dir);
  void stepOn();
  void stepOff();

  void stepSync(Stepper &s, int n1, int n2);
  void cmSync(Stepper &s, int  n1, int n2);
};

#endif
