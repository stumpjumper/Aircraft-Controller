#include "lucky7.h"
#include "IRremote.h"
#include "pins_arduino.h"

//IRrecv irrecv(IR);
//decode_results results;


void TimeOfDay::setup(uint16_t initialValueMin, uint16_t initialValueMax,
                 uint8_t nightDayThresholdPercentageValue )
{
  uint32_t now = millis();
  update30secTimeout = now + LUCKY7_TIME30SEC;
  update5minTimeout  = now + LUCKY7_TIME5MIN;
  eveningLength      = LUCKY7_TIME4HOUR;
  predawnLength      = LUCKY7_TIME2HOUR;
  morningLength      = LUCKY7_TIME2HOUR;
  photocellValuesIndex = 0;

  lengthOfNight = LUCKY7_TIME12HOUR;
  nightStart = 0;
  dayStart = 0;
  currentDayPart = DAY;

  nightDayThresholdPercentage = nightDayThresholdPercentageValue;

  photocellAvgValueMin = initialValueMin;
  photocellAvgValueMax = initialValueMax;
  photocellAvgValueCurrent = 0;

}


TimeOfDay::DayPart TimeOfDay::updateAverage(const uint16_t lightLevel)
{
  // Take reading every 30 seconds and record
  if (millis() >= update30secTimeout) {
    photocellValues[photocellValuesIndex] = lightLevel;
    photocellValuesIndex++;
    update30secTimeout = millis() + LUCKY7_TIME30SEC;
  }

  const uint8_t photocellValuesCount = photocellValuesIndex;

  // At five minutes
  if (millis() >= update5minTimeout) {
    // Throw out high and low
    uint8_t minIndex = 0;
    uint8_t maxIndex = 0;
    uint16_t min = photocellValues[0];
    uint16_t max = photocellValues[0];
    uint8_t i;    
    for (i = 1; i < photocellValuesCount; i++) {
      if (photocellValues[i] < min) {
        min = photocellValues[i];
        minIndex = i;
      }
      if (photocellValues[i] > max) {
        max = photocellValues[i];
        maxIndex = i;
      }
    }
    uint32_t sum = 0;
    uint8_t  numValues = 0;
    for (i = 0; i < photocellValuesCount; i++)
    {
      if (i != minIndex && i != maxIndex) {
        sum = sum + photocellValues[i];
        numValues++;
      }
    }
    // Average
    const uint16_t avg = (uint16_t)(sum/numValues);
    updatePhotocellAvgValues(avg);
    updateTimeOfDay();
    // Reset index and counter
    photocellValuesIndex = 0;
    update5minTimeout = millis() + LUCKY7_TIME5MIN;
  }

  return getDayPart();
}

void TimeOfDay::updatePhotocellAvgValues(uint16_t photocellAvgValue)
{
  photocellAvgValueCurrent = photocellAvgValue;
  
  if (photocellAvgValue > photocellAvgValueMax) {
    photocellAvgValueMax = photocellAvgValue;
  }
  
  if (photocellAvgValue < photocellAvgValueMin) {
    photocellAvgValueMin = photocellAvgValue;
  }
}

void TimeOfDay::updateTimeOfDay()
{
  const uint16_t nightDayThreshold = getNightDayThreshold();

  switch (currentDayPart) {
  case EVENING:
    if (millis() > nightStart + eveningLength) {
      currentDayPart = NIGHT;
    }
    break;
  case NIGHT:
    if (millis() > nightStart + lengthOfNight - predawnLength) {
      currentDayPart = PREDAWN;
    }
    // Yes, there is no break here.
  case PREDAWN:
    if (photocellAvgValueCurrent > nightDayThreshold) {
      dayStart = millis();
      lengthOfNight = dayStart - nightStart;
      currentDayPart = MORNING;
    }
    break;
  case MORNING:
    if (millis() > dayStart + morningLength) {
      currentDayPart = DAY;
    }
    break;
  case DAY:
    if (photocellAvgValueCurrent < nightDayThreshold) {
      nightStart = millis();
      currentDayPart = EVENING;
    }
    break;
  }    
}

uint16_t TimeOfDay::getNightDayThreshold()
{
  float percentage = nightDayThresholdPercentage/100.0;
  return (uint16_t)photocellAvgValueMin + percentage*(photocellAvgValueMax - photocellAvgValueMin);
}

TimeOfDay::DayPart TimeOfDay::getDayPart()
{
  return currentDayPart;
}

void UpDownMotor::setup(uint8_t * p_oUp, uint8_t * p_oDown)
{
  p_outputUp = p_oUp;
  p_outputDown = p_oDown;
  inMotorUpMode = false;
  inMotorDownMode = false;

  motorUpStartTime = 0;
  motorDownStartTime = 0;
}

void UpDownMotor::motorUpStart() {
    //Serial.print("In UpDownMotor::motorUpStart() \n");
    motorDownStop();
    if (!inMotorUpMode) {
      motorUpStartTime = millis();
    }
    inMotorUpMode = true;
}
void UpDownMotor::motorDownStart() {
    //Serial.print("In UpDownMotor::motorDownStart() \n");
    motorUpStop();
    if (!inMotorDownMode) {
      motorDownStartTime = millis();
    }
    inMotorDownMode = true;
}

void UpDownMotor::motorUpStop() {
    *p_outputUp = OFF;
    inMotorUpMode = false;
}

void UpDownMotor::motorDownStop() {
    *p_outputDown = OFF;
    inMotorDownMode = false;
}

void UpDownMotor::motorUpdate() {
    if ((inMotorUpMode && inMotorDownMode) || (*p_outputUp && *p_outputDown) )
    {
      Serial.print("ERROR: In UpDownMotor::motorUpdate() found ((inMotorUpMode && inMotorDownMode) || (*p_outputUp && *p_outputDown))\n");
      Serial.print("       Calling motorUpStop() and  motorDownStop()\n");
      motorUpStop();
      motorDownStop();
      return;
    }

    motorUpUpdate();
    motorDownUpdate();
}

void UpDownMotor::motorUpUpdate() {
    if (! inMotorUpMode) {
      return;
    }

    if (millis() > motorUpStartTime + LUCKY7_TIMEMOTORDELAY) {
      *p_outputUp = ON;
    }
      
    if (millis() > motorUpStartTime + LUCKY7_TIMEOUTMOTORUPDOWN) {
        motorUpStop();
    }
}

void UpDownMotor::motorDownUpdate() {
    if (! inMotorDownMode) {
      return;
    }

    if (millis() > motorDownStartTime + LUCKY7_TIMEMOTORDELAY) {
      *p_outputDown = ON;
    }
      
    if (millis() > motorDownStartTime + LUCKY7_TIMEOUTMOTORUPDOWN) {
        motorDownStop();
    }
}

void Lucky7::setup() {
  o1 = 0;
  o2 = 0;
  o3 = 0;
  o4 = 0;
  o5 = 0;
  o6 = 0;
  o7 = 0;
  o8 = 0;
  o13 = 0;

  pinMode(O1,OUTPUT);
  pinMode(O2,OUTPUT);
  pinMode(O3,OUTPUT);
  pinMode(O4,OUTPUT);
  pinMode(O5,OUTPUT);
  pinMode(O6,OUTPUT);
  pinMode(O7,OUTPUT);
  pinMode(O8,OUTPUT);
  pinMode(O13,OUTPUT);

  saveOutputState();

  irTimeout = 0;

  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  aveptr = 0;

  uint8_t i;
  for (i = 0; i < AVECNT; i++) {
    pc1[i] = 0;
    pc2[i] = 0;
    bc [i] = 0;
  }

  //irrecv.enableIRIn(); // Start the receiver
}

void Lucky7::saveOutputState()
{
  o1Saved = o1;
  o2Saved = o2; 
  o3Saved = o3; 
  o4Saved = o4; 
  o5Saved = o5; 
  o6Saved = o6; 
  o7Saved = o7; 
}

void Lucky7::setOutputStateFromSaved()
{
   o1 = o1Saved;
   o2 = o2Saved; 
   o3 = o3Saved; 
   o4 = o4Saved; 
   o5 = o5Saved; 
   o6 = o6Saved; 
   o7 = o7Saved; 
}


uint32_t Lucky7::loop() {
  uint8_t i = (aveptr++) % AVECNT;
  uint32_t rv = 0;

  analogWrite(O1,o1);
  analogWrite(O2,o2);
  analogWrite(O3,o3);
  digitalWrite(O4,o4);
  analogWrite(O5,o5);
  analogWrite(O6,o6);
  analogWrite(O7,o7);
  digitalWrite(O8,o8);
  digitalWrite(O13,o13);

  pc1[i] = analogRead(A4);
  pc2[i] = analogRead(A5);
  bc[i]  = analogRead(A0);

  if (millis() > irTimeout) { // Only look at ir remote value every irTimeout seconds
//      if (irrecv.decode(&results)) {
//      rv = results.value;
//      irTimeout = millis() + 500; // irTimeout = .5 seconds
//    }
//  } else {
//    irrecv.resume();
  }
  return rv;
}

uint16_t Lucky7::photocell1() {
    uint16_t sum = 0;
    uint8_t i;
    for (i = 0; i < AVECNT; i++) {
        sum += pc1[i];
    }
    return (uint16_t)(sum/AVECNT);
}

uint16_t Lucky7::photocell2() {
    uint32_t sum = 0;
    uint8_t i;
    for (i = 0; i < AVECNT; i++) {
        sum += pc2[i];
    }
    return (uint16_t)(sum/AVECNT);
}

float Lucky7::batteryVoltage() {
    uint32_t sum = 0;
    uint8_t i;
    for (i = 0; i < AVECNT; i++) {
        sum += bc[i];
    }
    return (float)(sum/AVECNT)*BVSCALE;
}

void Lucky7::o1MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O1, o1, targetValue, stepDelay);
}
void Lucky7::o2MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O2, o2, targetValue, stepDelay);
}
void Lucky7::o3MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O3, o3, targetValue, stepDelay);
}
void Lucky7::o4MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O4, o4, targetValue, stepDelay);
}
void Lucky7::o5MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O5, o5, targetValue, stepDelay);
}
void Lucky7::o6MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O6, o6, targetValue, stepDelay);
}
void Lucky7::o7MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O7, o7, targetValue, stepDelay);
}
void Lucky7::o8MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O8, o8, targetValue, stepDelay);
}
void Lucky7::o13MoveTo(uint8_t targetValue, const uint16_t stepDelay) {
  outputMoveTo(O13, o13, targetValue, stepDelay);
}

void Lucky7::outputMoveTo(const uint8_t outputPin, uint8_t & currentValue, uint8_t targetValue, const uint16_t stepDelay) {
  int16_t i;
  
  if (targetValue == currentValue) {
    return;
  }
  
  if (targetValue > currentValue) {
    for (i = currentValue; i <= targetValue; i++) {
      //Serial.println(i);
      analogWrite(outputPin,i);
      delayMicroseconds(stepDelay);
    }
  } else { // (currentValue > targetValue)
    for (i = currentValue; i >= targetValue; i--) {
      //Serial.println(i);
      analogWrite(outputPin,i);
      delayMicroseconds(stepDelay);
    }
  }
  currentValue = targetValue;
}

void Lucky7::boardLight(BoardLightMode mode, void (lightOn)(), void (lightOff)()) {
  uint32_t now10ms = millis() / 100;
  
  switch (mode) {
  case LIGHT_ON:
    lightOn();
    break;
  case LIGHT_OFF:
    lightOff();
    break;
  case LIGHT_SLOW_BLINK:
    if (now10ms % 10 == 0) {
      lightOn();
    } else {
      lightOff();
    }
    break;
  case LIGHT_FAST_BLINK:
    if (now10ms % 20 == 0) {
      lightOn();
    } else {
      lightOff();
    }
    break;
  }
}



