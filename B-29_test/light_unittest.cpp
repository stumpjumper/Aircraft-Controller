#include "lucky7.h"
#include <gtest/gtest.h>

const uint8_t onLightLevelValue = ON-2;

TEST(OnOffLight, Constructor) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.paused);
}

TEST(OnOffLight, On) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable, onLightLevelValue);

  EXPECT_EQ(false, light1.getPaused());
  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.on();
  EXPECT_EQ(onLightLevelValue, light1.lightLevel);
  EXPECT_EQ(onLightLevelValue, lightVariable);
  EXPECT_EQ(true, light1.getPaused());
}

TEST(OnOffLight, Off) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.off();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(true, light1.getPaused());
}

TEST(OnOffLight, FunctionCallOperatorGetValue) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1());
  EXPECT_EQ(ON, lightVariable);

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1());
  EXPECT_EQ(OFF, lightVariable);

  for (uint8_t i = 0; i < OFF; i++) {
    light1.lightLevel = i;
    EXPECT_EQ(i, light1());
    EXPECT_EQ(i, lightVariable);
  }
    
}

TEST(OnOffLight, FunctionCallOperatorSetValue) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  light1() = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);

  light1() = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);

  for (uint8_t i = 0; i < OFF; i++) {
    light1() = i;
    EXPECT_EQ(i, light1.lightLevel);
    EXPECT_EQ(i, lightVariable);
  }
    
}

TEST(OnOffLight, Resume) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(false, light1.getPaused());

  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.resume();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.resume();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.resume();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.resume();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(false, light1.getPaused());
}

TEST(OnOffLight, Update) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(false, light1.getPaused());

  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.update();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.update();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(true, light1.getPaused());

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.update();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.update();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(false, light1.getPaused());
}


TEST(DecayLight, Constructor)
{
  uint8_t lightVariable;

  uint32_t onLengths     [3] = {1000, 2000, 500};
  uint32_t decayLengths  [3] = { 500, 1000, 250};
  uint8_t  maxLightLevels[3] = { 100,  150, 200};
  uint32_t tauInMillisec [3] = { 250,  500, 125};
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 3);
  
  DecayLight light1(lightVariable,
                    onLightLevelValue,
                    numIntervals,
                    onLengths,
                    decayLengths,
                    maxLightLevels,
                    tauInMillisec);
  
  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF  , light1.lightLevel);
  EXPECT_EQ(OFF  , lightVariable);
  EXPECT_EQ(false, light1.getPaused());
  
  EXPECT_EQ(true , light1.decaying);
  EXPECT_EQ(0    , light1.changeTime);
  for (uint8_t i = 0; i < numIntervals; i++) {
    EXPECT_EQ(onLengths[i]     , light1.onLength[i]); 
    EXPECT_EQ(decayLengths[i]  , light1.decayLength[i]);
    EXPECT_EQ(maxLightLevels[i], light1.maxLightLevel[i]);
    EXPECT_EQ(tauInMillisec[i] , light1.tau[i]);
  }
}

TEST(DecayLight, Update)
{
  const uint8_t intervals = 3*24;
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(2*intervals-18);
  
  uint8_t lightVariable;


  uint32_t onLengths     [4] = { 500, 1000, 2000, 2000};
  uint32_t decayLengths  [4] = {1000, 2000, 4000, 4000};
  uint8_t  maxLightLevels[4] = { 100,  150,  200,  100};
  uint32_t tauInMillisec [4] = { 250,  500, 1000,    0};
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 4);

  
  DecayLight light1(lightVariable,
                    onLightLevelValue,
                    numIntervals,
                    onLengths,
                    decayLengths,
                    maxLightLevels,
                    tauInMillisec);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);


  // mSec   Decay  Level  index // mSec   Decay  Level  index 
  // ----   -----  -----  ----- // ----   -----  -----  ----- 
  // 1500   0      150    6     // 4500   0      200    12    
  // 2000   0      150    7     // 5500   0      200    13    
  // 2500   1      150    8     // 6500   1      200    14    
  // 3000   1       55    9     // 7500   1       74    15    
  // 3500   1       20    10    // 8500   1       27    16    
  // 4000   1        7    11    // 9500   1       10    17    

  //  mSec   Decay  Level  index // mSec   Decay  Level  index 
  //  ----   -----  -----  ----- // ----   -----  -----  ----- 
  // 10500   0      100    18    //  0     0      100    0     
  // 20500   0      100    19    //  250   0      100    1     
  // 30500   1      100    20    //  500   1      100    2     
  // 30500   1      100    21    //  750   1       37    3     
  // 40500   1      100    22    // 1000   1       14    4     
  // 50500   1      100    23    // 1250   1        5    5     

  // Expected Values:
 
  // NOTE: Becuase of the way DecayLight::update() does its indexing
  // the arrays passed to the constructor the when update() is called
  // for the frist time the array indexing is
  // 1..numIntervals-1
  // 0..numIntervals-1
  // 0..numIntervals-1
  // ...
  // That is why the expected values are off by one
  
  const uint16_t  timeSteps[4*6] = {500 ,  500,  500,  500,  500, 500,
                                    1000, 1000, 1000, 1000, 1000, 1000,
                                    1000, 1000, 1000, 1000, 1000, 1000,
                                    250 ,  250,  250,  250,  250, 250  };
  uint8_t  level[4*6] = {150, 150, 150, 55, 20,  7,
                         200, 200, 200, 74, 27, 10, 
                         100, 100, OFF,OFF,OFF,OFF,
                         100, 100, 100, 37, 14,  5 };
  bool     decaying[4*6] = {false, false, true, true, true, true,
                            false, false, true, true, true, true,
                            false, false, true, true, true, true,
                            false, false, true, true, true, true };
  
  uint32_t timeMS = 0;
  uint8_t i;
  uint8_t j;
  for (i = 0; i < intervals; i++) {
    j = i % sizeof(decaying)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);
    light1.update();
    EXPECT_EQ(level[j]   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(decaying[j], light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeSteps[j];
  }

  light1.on();

  uint8_t expectedLevel;
  bool    expectedDecaying;

  j = 6;
  for (i = 0; i < intervals; i++) {
    if (j == sizeof(decaying)/sizeof(bool))
      j = 0;
    arduinoMock->setMillisRaw(timeMS);

    if (i == 18)
      light1.resume();

    if (i < 18) {
      expectedLevel    = onLightLevelValue;
      expectedDecaying = true;
    } else {
      expectedLevel    = level[j];
      expectedDecaying = decaying[j];
    }
    
    light1.update();
    EXPECT_EQ(expectedLevel   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(expectedDecaying, light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeSteps[j];
    j++;
  }
  

  releaseArduinoMock();
}

TEST(DecayLight, UpdateWithTauNULL)
{
  const uint8_t intervals = 3*24;
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(2*intervals-18);
  
  uint8_t lightVariable;


  uint32_t onLengths     [4] = { 500, 1000, 2000, 2000};
  uint32_t decayLengths  [4] = {1000, 2000, 4000, 4000};
  uint8_t  maxLightLevels[4] = { 100,  150,  200,  100};
  uint32_t * tauInMillisec = NULL;
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 4);

  
  DecayLight light1(lightVariable,
                    onLightLevelValue,
                    numIntervals,
                    onLengths,
                    decayLengths,
                    maxLightLevels,
                    tauInMillisec);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  
  // mSec   Decay  Level  index // mSec   Decay  Level  index 
  // ----   -----  -----  ----- // ----   -----  -----  ----- 
  // 1500   0      150    6     // 4500   0      200    12    
  // 2000   0      150    7     // 5500   0      200    13    
  // 2500   1        0    8     // 6500   1        0    14    
  // 3000   1        0    9     // 7500   1        0    15    
  // 3500   1        0    10    // 8500   1        0    16    
  // 4000   1        0    11    // 9500   1        0    17    

  //  mSec   Decay  Level  index // mSec   Decay  Level  index 
  //  ----   -----  -----  ----- // ----   -----  -----  ----- 
  // 10500   0      100    18    //  0     0      100    0     
  // 20500   0      100    19    //  250   0      100    1     
  // 30500   1        0    20    //  500   1        0    2     
  // 30500   1        0    21    //  750   1        0    3     
  // 40500   1        0    22    // 1000   1        0    4     
  // 50500   1        0    23    // 1250   1        0    5     

  // Expected Values:
 
  // NOTE: Becuase of the way DecayLight::update() does its indexing
  // the arrays passed to the constructor the when update() is called
  // for the frist time the array indexing is
  // 1..numIntervals-1
  // 0..numIntervals-1
  // 0..numIntervals-1
  // ...
  // That is why the expected values are off by one
  
  const uint16_t  timeSteps[4*6] = {500 ,  500,  500,  500,  500, 500,
                                    1000, 1000, 1000, 1000, 1000, 1000,
                                    1000, 1000, 1000, 1000, 1000, 1000,
                                    250 ,  250,  250,  250,  250, 250  };
  uint8_t  level[4*6] = {150, 150, OFF, OFF, OFF, OFF,
                         200, 200, OFF, OFF, OFF, OFF,
                         100, 100, OFF, OFF, OFF, OFF,
                         100, 100, OFF, OFF, OFF, OFF };
  bool     decaying[4*6] = {false, false, true, true, true, true,
                            false, false, true, true, true, true,
                            false, false, true, true, true, true,
                            false, false, true, true, true, true };
  
  uint32_t timeMS = 0;
  uint8_t i;
  uint8_t j;
  for (i = 0; i < intervals; i++) {
    j = i % sizeof(decaying)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);
    light1.update();
    EXPECT_EQ(level[j]   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(decaying[j], light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeSteps[j];
  }

  light1.on();

  uint8_t expectedLevel;
  bool    expectedDecaying;

  j = 6;
  for (i = 0; i < intervals; i++) {
    if (j == sizeof(decaying)/sizeof(bool))
      j = 0;
    arduinoMock->setMillisRaw(timeMS);

    if (i == 18)
      light1.resume();

    if (i < 18) {
      expectedLevel    = onLightLevelValue;
      expectedDecaying = true;
    } else {
      expectedLevel    = level[j];
      expectedDecaying = decaying[j];
    }
    
    light1.update();
    EXPECT_EQ(expectedLevel   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(expectedDecaying, light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeSteps[j];
    j++;
  }
  

  releaseArduinoMock();
}

TEST(FlashingLight, Constructor)
{
  uint8_t lightVariable;

  uint32_t onLengths     [3] = {1000, 2000, 500};
  uint32_t decayLengths  [3] = { 500, 1000, 250};
  uint8_t  maxLightLevels[3] = { 100,  150, 200};
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 3);
  
  FlashingLight light1(lightVariable,
                    onLightLevelValue,
                    numIntervals,
                    onLengths,
                    decayLengths,
                    maxLightLevels);
  
  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF  , light1.lightLevel);
  EXPECT_EQ(OFF  , lightVariable);
  EXPECT_EQ(false, light1.getPaused());
  
  EXPECT_EQ(true , light1.decaying);
  EXPECT_EQ(0    , light1.changeTime);
  for (uint8_t i = 0; i < numIntervals; i++) {
    EXPECT_EQ(onLengths[i]     , light1.onLength[i]); 
    EXPECT_EQ(decayLengths[i]  , light1.decayLength[i]);
    EXPECT_EQ(maxLightLevels[i], light1.maxLightLevel[i]);
  }
}

TEST(BlinkingLight, Constructor)
{
  uint8_t lightVariable;

  BlinkingLight light1(lightVariable,
                       onLightLevelValue,
                       1000, // onLengthValue
                       10  , // offLengthValue
                       199); // maxLightLevelVaue

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(1000, light1.onLength[0]);
  EXPECT_EQ(10  , light1.decayLength[0]);
  EXPECT_EQ(199 , light1.maxLightLevel[0]);
}

TEST(BlinkingLight, Update)
{
  const uint8_t intervals = 3*24;
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(2*intervals-18);
  
  uint8_t lightVariable;


  uint32_t onLength      =  500;
  uint32_t offLength     = 1000; 
  uint8_t  maxLightLevel =  100; 
  
  BlinkingLight light1(lightVariable,
                       onLightLevelValue,
                       onLength,
                       offLength,
                       maxLightLevel);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);

  // mSec   Decay  Level  index 
  // ----   -----  -----  ----- 
  //  0     0      100    0     
  //  250   0      100    1     
  //  500   1        0    2     
  //  750   1        0    3     
  // 1000   1        0    4     
  // 1250   1        0    5     

  const uint16_t  timeStep = 250;
  uint8_t  level[6] = {100, 100, OFF, OFF, OFF, OFF };
  bool     off  [6] = {false, false, true, true, true, true};
  
  uint32_t timeMS = 0;
  uint8_t i;
  uint8_t j;
  for (i = 0; i < intervals; i++) {
    j = i % sizeof(off)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);
    light1.update();
    EXPECT_EQ(level[j], light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(off[j]  , light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeStep;
  }

  light1.on();

  uint8_t expectedLevel;
  bool    expectedOff;

  j = 6;
  for (i = 0; i < intervals; i++) {
    j = i % sizeof(off)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);
    
    if (i == 18)
      light1.resume();
    
    if (i < 18) {
      expectedLevel    = onLightLevelValue;
      expectedOff      = true;
    } else {
      expectedLevel    = level[j];
      expectedOff      = off[j];
    }
    
    light1.update();
    EXPECT_EQ(expectedLevel   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(expectedOff, light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeStep;
  }
  
  
  releaseArduinoMock();
}



TEST(FastBlinkingLight, Constructor)
{
  uint8_t lightVariable;

  FastBlinkingLight light1(lightVariable, onLightLevelValue,
                           199); // 199 is maxLightLevelVaue

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(1000, light1.onLength[0]);
  EXPECT_EQ(10  , light1.decayLength[0]);
  EXPECT_EQ(199 , light1.maxLightLevel[0]);
}

TEST(SlowBlinkingLight, Constructor)
{
  uint8_t lightVariable;

  SlowBlinkingLight light1(lightVariable, onLightLevelValue,
                           199); // 199 is maxLightLevelVaue

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(2000, light1.onLength[0]);
  EXPECT_EQ(10  , light1.decayLength[0]);
  EXPECT_EQ(199 , light1.maxLightLevel[0]);
}

