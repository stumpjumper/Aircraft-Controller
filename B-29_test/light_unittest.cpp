#include "lucky7.h"
#include <gtest/gtest.h>

TEST(OnOffLight, Constructor) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable);

  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.paused);
}

TEST(OnOffLight, On) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable);

  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.on();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(true, light1.getPaused());
}

TEST(OnOffLight, Off) {

  uint8_t lightVariable;

  OnOffLight light1(lightVariable);

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

  OnOffLight light1(lightVariable);

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

  OnOffLight light1(lightVariable);

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

  OnOffLight light1(lightVariable);

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

  OnOffLight light1(lightVariable);

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


TEST(BlinkingLight, Constructor)
{
  uint8_t lightVariable;

  BlinkingLight light1(lightVariable,
                       1000, // onLengthValue
                       10  , // offLengthValue
                       199); // maxLightLevelVaue

  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(1000, light1.onLength);
  EXPECT_EQ(10  , light1.offLength);
  EXPECT_EQ(199 , light1.maxLightLevel);
}


TEST(BlinkingLight, Update)
{
  const uint8_t aSize = 14;
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(2*aSize-4);
  
  uint8_t lightVariable;
  uint32_t onLengthValue  = 1000; // On for 1 second
  uint32_t offLengthValue =  500; // Off .5 seconds
  uint8_t  maxLightLevel  =  199; 
  
  BlinkingLight light1(lightVariable, onLengthValue,
                       offLengthValue, maxLightLevel);
  
  EXPECT_EQ(OFF, light1());
  EXPECT_EQ(false, light1.getPaused());
  EXPECT_EQ(0, light1.changeTime);
  
  uint8_t & mll = maxLightLevel;
  
  //                                 -on->          1 sec   -off->    1.5 sec
  uint32_t millis[aSize] = {0   , 1   , 500 , 999 , 1000, 1250, 1499, 1500,
                            //  -on->   2.5 sec   -off->  3 sec
                            2000, 2499, 2500, 2750, 2999, 3000};
  uint8_t  level [aSize] = {mll , mll,  mll , mll , OFF , OFF , OFF , mll ,
                            mll , mll , OFF , OFF , OFF , mll };
  //                        0     1     2     3     4     5     6     7
  //                        8     9     10    11    12    13    14    15

  for (uint8_t i = 0; i < aSize; i++)
  {
    arduinoMock->setMillisRaw(millis[i]);
    light1.update();
    EXPECT_EQ(level[i], light1()) << "i = " << int(i) << std::endl;
    EXPECT_EQ(level[i], lightVariable) << "i = " << int(i) << std::endl;
  }
  
  light1.on();
  
  for (uint8_t i = 0; i < aSize; i++)
  {
    uint8_t expectedValue;
    if (i < 4) {
      expectedValue = ON;
    } else {
      expectedValue = level[i];
    }
    
    if (i==4)
      light1.resume();
    
    arduinoMock->setMillisRaw(millis[i] + millis[aSize-1]);
    light1.update();
    EXPECT_EQ(expectedValue, light1()) << "i = " << int(i) << std::endl;
    EXPECT_EQ(expectedValue, lightVariable) << "i = " << int(i) << std::endl;
  }
    
  releaseArduinoMock();

}
  
TEST(FastBlinkingLight, Constructor)
{
  uint8_t lightVariable;

  FastBlinkingLight light1(lightVariable,
                           199); // 199 is maxLightLevelVaue

  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(1000, light1.onLength);
  EXPECT_EQ(10  , light1.offLength);
  EXPECT_EQ(199 , light1.maxLightLevel);
}

TEST(SlowBlinkingLight, Constructor)
{
  uint8_t lightVariable;

  SlowBlinkingLight light1(lightVariable,
                           199); // 199 is maxLightLevelVaue

  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(2000, light1.onLength);
  EXPECT_EQ(10  , light1.offLength);
  EXPECT_EQ(199 , light1.maxLightLevel);
}

TEST(DecayLight, Constructor)
{
  uint8_t lightVariable;

  const uint32_t onLengths     [3] = {1000, 2000, 500};
  const uint32_t decayLengths  [3] = { 500, 1000, 250};
  const uint8_t  maxLightLevels[3] = { 100,  150, 200};
  const uint32_t tauInMillisec [3] = { 250,  500, 125};
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 3);
  
  DecayLight light1(lightVariable,
                    numIntervals,
                    onLengths,
                    decayLengths,
                    maxLightLevels,
                    tauInMillisec);
  
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


  const uint32_t onLengths     [4] = { 500, 1000, 2000, 2000};
  const uint32_t decayLengths  [4] = {1000, 2000, 4000, 4000};
  const uint8_t  maxLightLevels[4] = { 100,  150,  200,  100};
  const uint32_t tauInMillisec [4] = { 250,  500, 1000,    0};
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 4);

  
  DecayLight light1(lightVariable,
                    numIntervals,
                    onLengths,
                    decayLengths,
                    maxLightLevels,
                    tauInMillisec);

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
      expectedLevel    = ON;
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

