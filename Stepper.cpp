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

void Stepper::setMaxDist(int cm){
  this->maxCm = cm;
}

int Stepper::getMaxDist(){
  return this->maxCm;
}

void Stepper::cm(int n){
  n = min(n, this->maxCm);
  this->step((long) n * this->step2Cm);
}

void Stepper::step(long n){
  bool neg = false;
  if(n < 0){
    neg = true;
    n *= -1;
    if(this->secondStepper != NULL) this->secondStepper->setDir(defaultDir);
    this->setDir(defaultDir);
  } else {
    if(this->secondStepper != NULL) this->secondStepper->setDir(!defaultDir);
    this->setDir(!defaultDir);
    n = min((long) this->maxCm*this->step2Cm - this->pos, n);
  }
  
  long s = 0;
  for(n; n>0; n--){
    if(!digitalRead(this->lsMin) && neg || !digitalRead(this->lsMax) && !neg) break;

    s++;
    if(secondStepper != NULL) secondStepper->stepOn();
    digitalWrite(this->stepPin, 1);
    delayMicroseconds(this->delaySpeed);
    
    if(secondStepper != NULL) secondStepper->stepOff();
    digitalWrite(this->stepPin, 0);
    delayMicroseconds(this->delaySpeed);
  }
  
  if(neg) this->pos -= s;
  else this->pos += s;

  if(!digitalRead(this->lsMin)) this->pos = 0;
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

void Stepper::stepSync(Stepper& s, long n1, long n2){
  bool pNeg = false;
  bool sNeg = false;

  if(n1 < 0){
    pNeg = true;
    n1 *= -1;
    this->setDir(this->defaultDir);
    if(this->secondStepper != NULL) this->secondStepper->setDir(this->secondStepper->defaultDir);
  } else {
    this->setDir(!this->defaultDir);
    if(this->secondStepper != NULL) this->secondStepper->setDir(!this->secondStepper->defaultDir);
  }
  if(n2 < 0){
    sNeg = true;
    n2 *= -1;
    s.setDir(s.defaultDir);
    if(s.secondStepper != NULL) s.secondStepper->setDir(s.secondStepper->defaultDir);
  } else {
    s.setDir(!s.defaultDir);
    if(s.secondStepper != NULL) s.secondStepper->setDir(!s.secondStepper->defaultDir);
  }

  long s1 = 0, s2 = 0;
  unsigned long delayMax = max(this->delaySpeed * 2 * n1, s.delaySpeed * 2 * n2);
  unsigned long delayA = (delayMax / n1) / 2;
  unsigned long delayB = (delayMax / n2) / 2;

//  Serial.println((String) delayMax + '\t' + delayA + '\t' + delayB + '\t' + (this->delaySpeed * 2 * n1) + '\t' + n2);

  bool stateA = 1, stateB = 1;
  unsigned long timingA = micros();
  unsigned long timingB = micros();
  while(n1 > 0 || n2 > 0){
    bool canStep1 = true, canStep2 = true;
    
    if((unsigned long) micros()-timingA > delayA && n1 > 0){
      timingA = micros();
      if(!digitalRead(this->lsMin) && pNeg || !digitalRead(this->lsMax) && !pNeg) canStep1 = false;
      
      if(stateA && canStep1){
        this->stepOn();
        if(this->secondStepper != NULL) this->secondStepper->stepOn();
      } else {
        this->stepOff();
        if(this->secondStepper != NULL) this->secondStepper->stepOff();
      }
      stateA = !stateA;
      if(stateA == 1){
        if(canStep1) s1++;
        n1--;
      }
    }
    if((unsigned long) micros()-timingB > delayB && n2 > 0){
      timingB = micros();
      if(!digitalRead(s.lsMin) && sNeg || !digitalRead(s.lsMax) && !sNeg) canStep2 = false;
      
      if(stateB && canStep2){
        s.stepOn();
        if(s.secondStepper != NULL) s.secondStepper->stepOn();
      } else {
        s.stepOff();
        if(s.secondStepper != NULL) s.secondStepper->stepOff();
      }
      stateB = !stateB;
      if(stateB == 1){
        if(canStep2) s2++;
        n2--;
      }
    }
  }
  
  if(pNeg) this->pos -= s1;
  else this->pos += s1;
  
  if(sNeg) s.pos -= s2;
  else s.pos += s2;
  
  if(!digitalRead(this->lsMin)) this->pos = 0;
  if(!digitalRead(s.lsMin)) s.pos = 0;
}

void Stepper::cmSync(Stepper &s, int  n1, int n2){
  this->stepSync(s, n1 * this->step2Cm, n2 * s.step2Cm);
}
