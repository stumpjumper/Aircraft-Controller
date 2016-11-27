#include "lucky7.h"
#include "pins_arduino.h"
#ifdef DOING_UNIT_TESTING
  #include <IRremote.h>
#else
  #include "IRremote.h"
#endif

IRrecv irRecv(IR);
decode_results irResults;

Light::Light() : p_lightLevel(NULL), lightMode(LIGHT_MODE_NOTSET) {;}
Light::~Light() {;}

void Light::setup(uint8_t & lightLevelVariable, const uint8_t onLightLevelValue)
{
  p_lightLevel = &lightLevelVariable;
  onLightLevel = onLightLevelValue;
  off();
}

void Light::setOnLightLevel(const uint8_t onLightLevelValue)
{
  onLightLevel = onLightLevelValue;
  if (LIGHT_ON == lightMode) {
    on();
  }
}

void Light::incrementOnLightLevel(const int16_t onLightLevelIncrement)
{
  int16_t onLightLevelValue = onLightLevel + onLightLevelIncrement;
  if (onLightLevelValue > ON)
  {
    onLightLevelValue = ON;
  }
  else if (onLightLevelValue < OFF)
  {
    onLightLevelValue = OFF;
  }
  
  setOnLightLevel(onLightLevelValue);
}


void Light::toggle() {
  switch (lightMode) {
  case LIGHT_MODE_NOTSET:
  case LIGHT_FLASHING:
    // off();
    break;
  case LIGHT_OFF:
    on();
    break;
  case LIGHT_ON:
    off();
    // flash();
    break;
  }
}

BlinkingLight::BlinkingLight() {;}
BlinkingLight::~BlinkingLight() {;}
void BlinkingLight::setup(uint8_t & lightLevelVariable,
                          const uint8_t onLightLevelValue,
                          const uint32_t onLengthValue,
                          const uint32_t offLengthValue,
                          const uint8_t maxLightLevelValue)
{
  FlashingLight::setup(lightLevelVariable, onLightLevelValue, 1, onLengthValues,                        offLengthValues, maxLightLevelValues);
  onLengthValues[0] = onLengthValue;
  offLengthValues[0] = offLengthValue;
  maxLightLevelValues[0] = maxLightLevelValue;
}

FastBlinkingLight::FastBlinkingLight() {;}
FastBlinkingLight::~FastBlinkingLight() {;}
void FastBlinkingLight::setup(uint8_t  & lightLevelVariable,
                              const uint8_t onLightLevelValue,
                              const uint8_t maxLightLevelValue)
{
  BlinkingLight::setup(lightLevelVariable, // Where the light level will be stored
                       onLightLevelValue, // Brightness when light is constant on
                       onLengthValue,
                       offLengthValue,
                       maxLightLevelValue);
}

SlowBlinkingLight::SlowBlinkingLight() {;}
SlowBlinkingLight::~SlowBlinkingLight() {;}
void SlowBlinkingLight::setup(uint8_t  & lightLevelVariable,
                              const uint8_t onLightLevelValue,
                              const uint8_t maxLightLevelValue)
{
  BlinkingLight::setup(lightLevelVariable, // Where the light level will be stored
                       onLightLevelValue, // Brightness when light is constant on
                       onLengthValue,
                       offLengthValue,
                       maxLightLevelValue);
}

FastSlowBlinkingLight::FastSlowBlinkingLight() : p_currentLight(NULL),blinkSpeed(NOTSET) {;}
FastSlowBlinkingLight::~FastSlowBlinkingLight() {;}
void FastSlowBlinkingLight::setup(uint8_t & lightLevelVariable,
                                  const uint8_t onLightLevel,
                                  const uint8_t maxLightLevelValue)
{
  fastLight.setup(lightLevelVariable, onLightLevel, maxLightLevelValue);
  slowLight.setup(lightLevelVariable, onLightLevel, maxLightLevelValue);
  setToFast();
}

DecayLight::DecayLight() :
  changeTime(0), decaying(false), decayStartTime(0), intervalIndex(0), 
  numIntervals(0), 
  onLength(NULL), decayLength(NULL), maxLightLevel(NULL), tau(NULL) {;}
DecayLight::~DecayLight() {;}
void DecayLight::setup(uint8_t  & lightLevelVariable,
                       const uint8_t onLightLevelValue,
                       const uint8_t numberOfValues,
                       uint32_t * onLengthValues,
                       uint32_t * decayLengthValues,
                       uint8_t  * maxLightLevelValues,
                       uint32_t * tauInMilliseconds)
{
  Light::setup(lightLevelVariable, onLightLevelValue);

  // Overide call to off() made in Light::setup since we want this light
  // to be in flashing mode right away
  *p_lightLevel = OFF; // Set the initial light level
  lightMode = LIGHT_FLASHING;

  onLength = onLengthValues;
  decayLength = decayLengthValues;
  maxLightLevel = maxLightLevelValues;
  tau = tauInMilliseconds;
  numIntervals = numberOfValues;

  changeTime     = 0;    // Change right away
  decaying       = true; // Will cause us to go to on mode right away
  decayStartTime = 0;   
  intervalIndex  = 0;   // Will be incremented during first call to update
}

void DecayLight::update()
{
  //Serial.println(F("In update() A"));
  uint8_t j;

  uint32_t changeTimeDelta = 0;
  
  const uint32_t now = millis();
  j = intervalIndex % numIntervals;
  
  if (now >= changeTime) {
    if (decaying) {
      //Serial.println(F("In update() B"));
      decaying = false;
      intervalIndex++;
      j = intervalIndex % numIntervals;
      if (lightMode == LIGHT_FLASHING) {
        *p_lightLevel = maxLightLevel[j];
      }
      changeTimeDelta = onLength[j];
    } else {
      //Serial.println(F("In update() C"));
      decaying = true;
      changeTimeDelta = decayLength[j];
    }
    decayStartTime = changeTime;
    changeTime = changeTime + changeTimeDelta;
    // Check if time between calls to update() is > onLength[j] or decayLength[j]
    if (now >= changeTime) { 
      //Serial.println(F("In update() D"));
      decayStartTime = now;
      changeTime = now + changeTimeDelta;
      // std::cerr << "decayStartTime, changeTime, decaying = " << decayStartTime << ", " << changeTime << ", " <<  decaying << std::endl;
    }
  }

  //Serial.println(F("In update() E"));
  if (decaying && lightMode == LIGHT_FLASHING) {
    //std::cerr << "A" << std::endl;
    if (tau == NULL || tau[j] == 0) {
      //std::cerr << "B" << std::endl;
      *p_lightLevel = OFF;
    } else {
      //Serial.println(F("In update() F"));
      //std::cerr << "C" << std::endl;
      const uint32_t time = now - decayStartTime;
      //std::cerr << "now " << int(now) << std::endl;
      //std::cerr << "time " << int(time) << std::endl;
      // T = dT*e(-t/tau)  // T = Dt @ t=0, T = 0 @ t = infinity
      *p_lightLevel =
        uint8_t(float(maxLightLevel[j])*exp(-float(time)/float(tau[j]))+.5);
      //std::cerr << "lightLevel " << int(lightLevel) << std::endl;
    }
    //Serial.println(F("In update() G"));
  }

  // std::cerr << "now = " << now
  //           << " decayStartTime = " << decayStartTime
  //           << " changeTime  = " << changeTime
  //           << " numIntervals = " << int(numIntervals)
  //           << " intervalIndex = " << int(intervalIndex)
  //           << " j = " << int(j) << std::endl
  //           << "decaying = " << decaying
  //           << " lightLevel = " << int(lightLevel)
  //           << " tau  = " << (tau != NULL ? int(tau[j]) : 0)
  //           << std::endl;
}

RotatingLight::RotatingLight() :
  flatLength(0), 
  flatLightLevel(0), 
  pulseLength(0), 
  minLightLevel(0), 
  maxLightLevel(0), 
  changeTime(0),
  pulsing(LIGHT_MODE_NOTSET),
  pulseStartTime(0) {;}

RotatingLight::~RotatingLight() {;}
void RotatingLight::setup(uint8_t & lightLevelVariable,    
                          const uint8_t  onLightLevelValue,
                          const uint32_t flatLengthValue,           
                          const uint8_t  flatLightLevelValue,       
                          const uint32_t pulseLengthValue,     
                          const uint8_t  minLightLevelValue,
                          const uint8_t  maxLightLevelValue)
{
  Light::setup(lightLevelVariable, onLightLevelValue);

  // Overide call to off() made in Light::setup since we want this light
  // to be in flashing mode right away
  *p_lightLevel = OFF; // Set the initial light level
  lightMode = LIGHT_FLASHING;

  flatLength      = flatLengthValue;
  flatLightLevel  = flatLightLevelValue;
  pulseLength     = pulseLengthValue;
  minLightLevel   = minLightLevelValue;
  maxLightLevel   = maxLightLevelValue;

  changeTime     = 0;    // Change right away
  pulsing        = true; // Will cause us to go to pulsing mode right away
  pulseStartTime = 0;   
}


void RotatingLight::update()
{

  uint32_t changeTimeDelta = 0;
  
  const uint32_t now = millis();
  
  if (now >= changeTime) {
    if (pulsing) {
      pulsing = false;
      if (lightMode == LIGHT_FLASHING) {
        *p_lightLevel = flatLightLevel;
      }
      changeTimeDelta = flatLength;
    } else {
      pulsing = true;
      changeTimeDelta = pulseLength;
    }
    pulseStartTime = changeTime;
    changeTime = changeTime + changeTimeDelta;
    // Check if time between calls to update() is > offLength or pulseLength
    if (now >= changeTime) { 
      pulseStartTime = now;
      changeTime = now + changeTimeDelta;
      // std::cerr << "pulseStartTime, changeTime, pulsing = " << pulseStartTime << ", " << changeTime << ", " <<  pulsing << std::endl;
    }
  }

  if (pulsing && lightMode == LIGHT_FLASHING) {
    //std::cerr << "A" << std::endl;
    if (pulseLength == 0) {
      //std::cerr << "B" << std::endl;
      *p_lightLevel = maxLightLevel;
    } else {
      //std::cerr << "C" << std::endl;
      const uint32_t time = now - pulseStartTime;
      //std::cerr << "now " << int(now) << std::endl;
      //std::cerr << "time " << int(time) << std::endl;
      // T = dT*e(-t/tau)  // T = Dt @ t=0, T = 0 @ t = infinity
      *p_lightLevel =
        uint8_t(float(maxLightLevel-minLightLevel)*fabs(sin(float(time)*3.1415926536/float(pulseLength)))) + minLightLevel;
      //std::cerr << "lightLevel " << int(lightLevel) << std::endl;
    }
  }

  // std::cerr << "now = " << now
  //           << " pulseStartTime = " << pulseStartTime
  //           << " changeTime  = " << changeTime
  //           << " numIntervals = " << int(numIntervals)
  //           << " intervalIndex = " << int(intervalIndex)
  //           << "pulsing = " << pulsing
  //           << " lightLevel = " << int(lightLevel)
  //           << " tau  = " << (tau != NULL ? int(tau[j]) : 0)
  //           << std::endl;
}

FlashingLight::FlashingLight() {;}
FlashingLight::~FlashingLight() {;}
void FlashingLight::setup(uint8_t & lightLevelVariable,
                          const uint8_t onLightLevelValue,
                          const uint8_t numberOfValues,
                          uint32_t * onLengthValues,
                          uint32_t * offLengthValues,
                          uint8_t  * maxLightLevelValues)
{
  DecayLight::setup(lightLevelVariable, onLightLevelValue, numberOfValues, onLengthValues,
                   offLengthValues, maxLightLevelValues, NULL);
}

void TimeOfDay::setup(const uint16_t initialValueMin,
                      const uint16_t initialValueMax,
                      const uint8_t nightDayThresholdPercentageValue )
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

  updateAverageTestMode = false;
}


TimeOfDay::DayPart TimeOfDay::updateAverage(const uint16_t lightLevel)
{
  if (updateAverageTestMode) {
    return getDayPart();
  }
  
  // Take reading every 30 seconds and record
  if (millis() >= update30secTimeout &&
      photocellValuesIndex < PHOTOCELLVALUESSIZE) {
    photocellValues[photocellValuesIndex] = lightLevel;
    photocellValuesIndex++;
    update30secTimeout = millis() + LUCKY7_TIME30SEC;
  }
  
  const uint8_t photocellValuesCount = photocellValuesIndex;

  // At five minutes
  if (millis() >= update5minTimeout) {
#ifdef DOING_UNIT_TESTING
    // Can easily happen during testing.  If you hit this, look into setting
    // the updateAverageTestMode flag via setUpdateAverageTestMode(true);
    assert(photocellValuesCount > 2);
#endif
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
    // NOTE: If photocellValuesCount < 3 numValues could be 0
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

void TimeOfDay::setUpdateAverageTestMode(bool testModeFlag) {
  updateAverageTestMode = testModeFlag;
}

void UpDownMotor::setup(uint8_t & oUp, uint8_t & oDown)
{
  p_outputUp = &oUp;
  p_outputDown = &oDown;
  inMotorUpMode = false;
  inMotorDownMode = false;

  *p_outputUp   = OFF; // Up motor off to start
  *p_outputDown = OFF; // Down motor off to start

  motorUpStartTime = 0;
  motorDownStartTime = 0;
}

void UpDownMotor::motorUpStart() {
    //Serial.print(F("In UpDownMotor::motorUpStart() \n"));
    motorDownStop();
    if (!inMotorUpMode) {
      motorUpStartTime = millis();
    }
    inMotorUpMode = true;
}
void UpDownMotor::motorDownStart() {
    //Serial.print(F("In UpDownMotor::motorDownStart() \n"));
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
      Serial.println(F("ERROR: In UpDownMotor::motorUpdate() found ((inMotorUpMode && inMotorDownMode) || (*p_outputUp && *p_outputDown))"));
      Serial.println(F("       Calling motorUpStop() and motorDownStop()"));
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

  irRecv.enableIRIn(); // Start the receiver
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

  // std::cout << "analogRead(A4) pc1[" << int(i) << "] =" << pc1[i] << std::endl;
  // std::cout << "analogRead(A5) pc2[" << int(i) << "] =" << pc2[i] << std::endl;
  // std::cout << "analogRead(A0)  bc[" << int(i) << "] =" << bc[i] << std::endl;
  
  rv = irLoop();
  return rv;
}

uint32_t Lucky7::irLoop() {
  uint32_t rv = 0;
  
  const uint32_t currentMillis = millis();
  if (currentMillis > irTimeout) { // Only look at ir remote value every irTimeout seconds
    if (irRecv.decode(&irResults)) {
      rv = irResults.value;
      irTimeout = currentMillis + 500; // irTimeout = .5 seconds
    }
  } else {
    irRecv.resume();
  }
  
  return rv;
}

uint16_t Lucky7::photocell(uint16_t * pc) {
  uint16_t sum = 0;
  uint8_t i;
  for (i = 0; i < AVECNT; i++) {
    //std::cout << "pc[" << int(i) << "] =" << pc[i] << std::endl;
    sum += pc[i];
  }
  return (uint16_t)(sum/AVECNT);
}

uint16_t Lucky7::photocell1() {
  return photocell(pc1);
}

uint16_t Lucky7::photocell2() {
  return photocell(pc2);
}

float Lucky7::batteryVoltage() {
  uint32_t sum = 0;
  uint8_t i;
  for (i = 0; i < AVECNT; i++) {
    //std::cout << "bc[" << int(i) << "] =" << bc[i] << std::endl;
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

