#include "Stepper.h"

Stepper::Stepper(uint8_t sPin, uint8_t dPin, uint8_t ePin, uint8_t lMin, uint8_t lMax){
   this->stepPin = sPin;
   this->dirPin = dPin;
   this->enPin = ePin;
   this->lsMin = lMin;
   this->lsMax = lMax;
}

void Stepper::begin(){
   pinMode(this->stepPin, OUTPUT);
   pinMode(this->dirPin, OUTPUT);
   pinMode(this->enPin, OUTPUT);
   pinMode(this->lsMin, INPUT_PULLUP);
   pinMode(this->lsMax, INPUT_PULLUP);
   
   digitalWrite(this->stepPin, 0);
   digitalWrite(this->enPin, 0);
}

void Stepper::disable(){
  digitalWrite(this->enPin, 1);
}

void Stepper::enable(){
  digitalWrite(this->enPin, 0);
}

void Stepper::setDelaySpeed(uint16_t dSpeed){
  this->delaySpeed = dSpeed;
}
  
void Stepper::step(int n){
  this->pos += n;
  
  if(n < 0){
    n *= -1;
    if(this->secondStepper != NULL) this->secondStepper->setDir(defaultDir);
    this->setDir(defaultDir);
    if(digitalRead(this->lsMin) == 0) return;
  } else {
    if(this->secondStepper != NULL) this->secondStepper->setDir(!defaultDir);
    this->setDir(!defaultDir);
    if(digitalRead(this->lsMax) == 0) return;
  }
  
  for(n; n>0; n--){
    if(secondStepper != NULL) secondStepper->stepOn();
    digitalWrite(this->stepPin, 1);
    delayMicroseconds(this->delaySpeed);
    
    if(secondStepper != NULL) secondStepper->stepOff();
    digitalWrite(this->stepPin, 0);
    delayMicroseconds(this->delaySpeed);
  }
}

void Stepper::cm(int n){
  this->step(n * this->step2Cm);
}

void Stepper::setStep2Cm(int n){
  this->step2Cm = n;
}

void Stepper::resetPosition(){
  this->pos = 0;
}

long Stepper::getPosition(){
  return this->pos;
}

int Stepper::getPositionInCm(){
  return this->pos / this->step2Cm;
}

void Stepper::home(){
  while(digitalRead(this->lsMin)){
    this->step(-1);
  }
  this->resetPosition();
}

void Stepper::join(Stepper& s){
  this->secondStepper = &s;
}

void Stepper::toggleDir(){
  this->defaultDir = !this->defaultDir;
}

void Stepper::setDir(bool dir){
  digitalWrite(this->dirPin, (this->defaultDir ? dir : !dir));
}

void Stepper::stepOn(){
  digitalWrite(this->stepPin, 1);
}

void Stepper::stepOff(){
  digitalWrite(this->stepPin, 0);
}

void Stepper::stepSync(Stepper& s, int n1, int n2){
  this->setDir(n1 < 0 ? this->defaultDir : !this->defaultDir);
  if(this->secondStepper != NULL) this->secondStepper->setDir(n1 < 0 ? this->secondStepper->defaultDir : !this->secondStepper->defaultDir);
  
  s.setDir(n2 < 0 ? s.defaultDir : !s.defaultDir);
  if(s.secondStepper != NULL) s.secondStepper->setDir(n1 < 0 ? s.secondStepper->defaultDir : !s.secondStepper->defaultDir);

  n1 = abs(n1);
  n2 = abs(n2);
  unsigned long delayMax = max(this->delaySpeed * 2 * n1, s.delaySpeed * 2 * n2);
  unsigned long delayA = (delayMax / n1) / 2;
  unsigned long delayB = (delayMax / n2) / 2;

  Serial.println((String) delayMax + '\t' + delayA + '\t' + delayB + '\t' + (this->delaySpeed * 2 * n1) + '\t' + n2);

  bool stateA = 1, stateB = 1;
  unsigned long timingA = micros();
  unsigned long timingB = micros();
  while(n1 > 0 || n2 > 0){
    if((unsigned long) micros()-timingA > delayA && n1 > 0){
      timingA = micros();
      if(stateA){
        this->stepOn();
        if(this->secondStepper != NULL) this->secondStepper->stepOn();
      } else {
        this->stepOff();
        if(this->secondStepper != NULL) this->secondStepper->stepOff();
      }
      stateA = !stateA;
      if(stateA == 1) n1--;
    }
    if((unsigned long) micros()-timingB > delayB && n2 > 0){
      timingB = micros();
      if(stateB){
        s.stepOn();
        if(s.secondStepper != NULL) s.secondStepper->stepOn();
      } else {
        s.stepOff();
        if(s.secondStepper != NULL) s.secondStepper->stepOff();
      }
      stateB = !stateB;
      if(stateB == 1) n2--;
    }
  }
}
