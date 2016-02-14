#include "lucky7.h"
#include <gtest/gtest.h>

using std::setfill;
using std::setw;

const uint8_t onLightLevelValue = ON-2;
const uint8_t maxLightLevelValue = 199;

const uint32_t fastOnLengthValue  = FastBlinkingLight::onLengthValue;
const uint32_t fastOffLengthValue = FastBlinkingLight::offLengthValue;

const uint32_t slowOnLengthValue  = SlowBlinkingLight::onLengthValue;
const uint32_t slowOffLengthValue = SlowBlinkingLight::offLengthValue;

TEST(Light, Constructor) {

  uint8_t lightVariable;

  Light light1;
  EXPECT_EQ(NULL, light1.p_lightLevel);
  EXPECT_EQ(Light::LIGHT_MODE_NOTSET, light1.lightMode);
  light1.setup(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.paused);
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.lightMode);
}

TEST(Light, Toggle) {

  uint8_t lightVariable;

  Light light1;
  EXPECT_EQ(Light::LIGHT_MODE_NOTSET, light1.getLightMode());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_MODE_NOTSET, light1.getLightMode());

  light1.setup(lightVariable, onLightLevelValue);
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());

  light1.on();
  EXPECT_EQ(Light::LIGHT_ON, light1.getLightMode());
  EXPECT_EQ(onLightLevelValue, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_OFF, light1.getLightMode());
  EXPECT_EQ(OFF, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_ON, light1.getLightMode());
  EXPECT_EQ(onLightLevelValue, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_OFF, light1.getLightMode());
  EXPECT_EQ(OFF, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_ON, light1.getLightMode());
  EXPECT_EQ(onLightLevelValue, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_OFF, light1.getLightMode());
  EXPECT_EQ(OFF, light1());

  light1.resume();
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());

  light1.off();
  EXPECT_EQ(Light::LIGHT_OFF, light1.getLightMode());
  EXPECT_EQ(OFF, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_ON, light1.getLightMode());
  EXPECT_EQ(onLightLevelValue, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_OFF, light1.getLightMode());
  EXPECT_EQ(OFF, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_ON, light1.getLightMode());
  EXPECT_EQ(onLightLevelValue, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_OFF, light1.getLightMode());
  EXPECT_EQ(OFF, light1());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_ON, light1.getLightMode());
  EXPECT_EQ(onLightLevelValue, light1());

  light1.resume();
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());
  light1.toggle();
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());

}


TEST(Light, LightModes) {

  uint8_t lightVariable;

  Light light1;
  EXPECT_EQ(Light::LIGHT_MODE_NOTSET, light1.getLightMode());

  light1.setup(lightVariable, onLightLevelValue);
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());

  light1.on();
  EXPECT_EQ(Light::LIGHT_ON, light1.getLightMode());

  light1.resume();
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());

  light1.off();
  EXPECT_EQ(Light::LIGHT_OFF, light1.getLightMode());

  light1.resume();
  EXPECT_EQ(Light::LIGHT_FLASHING, light1.getLightMode());

}

TEST(Light, On) {

  uint8_t lightVariable;

  Light light1;
  light1.setup(lightVariable, onLightLevelValue);

  EXPECT_EQ(false, light1.getPaused());
  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);

  *light1.p_lightLevel = OFF;
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.on();
  EXPECT_EQ(onLightLevelValue, *light1.p_lightLevel);
  EXPECT_EQ(onLightLevelValue, lightVariable);
  EXPECT_EQ(true, light1.getPaused());
}

TEST(Light, Off) {

  uint8_t lightVariable;

  Light light1;
  light1.setup(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(false, light1.getPaused());

  *light1.p_lightLevel = ON;
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.off();
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(true, light1.getPaused());
}

TEST(Light, FunctionCallOperatorGetValue) {

  uint8_t lightVariable;

  Light light1;
  light1.setup(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  *light1.p_lightLevel = ON;
  EXPECT_EQ(ON, light1());
  EXPECT_EQ(ON, lightVariable);

  *light1.p_lightLevel = OFF;
  EXPECT_EQ(OFF, light1());
  EXPECT_EQ(OFF, lightVariable);

  for (uint8_t i = 0; i < OFF; i++) {
    *light1.p_lightLevel = i;
    EXPECT_EQ(i, light1());
    EXPECT_EQ(i, lightVariable);
  }
    
}

TEST(Light, FunctionCallOperatorSetValue) {

  uint8_t lightVariable;

  Light light1;
  light1.setup(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  light1() = ON;
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);

  light1() = OFF;
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);

  for (uint8_t i = 0; i < OFF; i++) {
    light1() = i;
    EXPECT_EQ(i, *light1.p_lightLevel);
    EXPECT_EQ(i, lightVariable);
  }
    
}

TEST(Light, Resume) {

  uint8_t lightVariable;

  Light light1;
  light1.setup(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(false, light1.getPaused());

  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  *light1.p_lightLevel = OFF;
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.resume();
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  *light1.p_lightLevel = ON;
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.resume();
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  *light1.p_lightLevel = OFF;
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.resume();
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  *light1.p_lightLevel = ON;
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.resume();
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(false, light1.getPaused());
}

TEST(Light, Update) {

  uint8_t lightVariable;

  Light light1;
  light1.setup(lightVariable, onLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(false, light1.getPaused());

  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  *light1.p_lightLevel = OFF;
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.update();
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(true, light1.getPaused());

  *light1.p_lightLevel = ON;
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.update();
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  EXPECT_EQ(true, light1.getPaused());

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  *light1.p_lightLevel = OFF;
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  light1.update();
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  *light1.p_lightLevel = ON;
  EXPECT_EQ(ON, *light1.p_lightLevel);
  EXPECT_EQ(ON, lightVariable);
  light1.update();
  EXPECT_EQ(ON, *light1.p_lightLevel);
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
  
  DecayLight light1;
  EXPECT_EQ(NULL, light1.onLength     );
  EXPECT_EQ(NULL, light1.decayLength  );
  EXPECT_EQ(NULL, light1.maxLightLevel);
  EXPECT_EQ(NULL, light1.tau          );
  light1.setup(lightVariable,
               onLightLevelValue,
               numIntervals,
               onLengths,
               decayLengths,
               maxLightLevels,
               tauInMillisec);
  
  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF  , *light1.p_lightLevel);
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
    .Times(2*intervals);
  
  uint8_t lightVariable;

  // Expected Values:

  //  i   j   mSec   Decaying Light Level
  // --  --   ----   -------- -----------
  //  0   0   0         0     150  Timestep = 500
  //  1   1   500       0     150  onLength = 1000   
  //  2   2   1000      1     150  decayLength = 2000
  //  3   3   1500      1     55   maxLightLeve = 150
  //  4   4   2000      1     20   tau = 500         
  //  5   5   2500      1     7
  
  //  6   6   3000      0     200  Timestep = 1000
  //  7   7   4000      0     200  onLength = 2000   
  //  8   8   5000      1     200  decayLength = 4000
  //  9   9   6000      1     74   maxLightLeve = 200
  // 10   10  7000      1     27   tau = 1000        
  // 11   11  8000      1     10
  
  // 12   12  9000      0     100  Timestep = 1000
  // 13   13  10000     0     100  onLength = 2000   
  // 14   14  11000     1     0    decayLength = 4000
  // 15   15  12000     1     0    maxLightLeve = 100
  // 16   16  13000     1     0    tau = 0           
  // 17   17  14000     1     0
  
  // 18   18  15000     0     100  Timestep = 250
  // 19   19  15250     0     100  onLength = 500    
  // 20   20  15500     1     100  decayLength = 1000
  // 21   21  15750     1     37   maxLightLeve = 100
  // 22   22  16000     1     14   tau = 250         
  // 23   23  16250     1     5
 
  // NOTE: Becuase of the way DecayLight::update() does its indexing
  // the arrays passed to the constructor the when update() is called
  // for the frist time the array indexing is
  // 1..numIntervals-1
  // 0..numIntervals-1
  // 0..numIntervals-1
  // ...
  // That is why the expected values are off by one

  //                                   ---------------- Starts here
  //                                  \/
  uint32_t onLengths     [4] = { 500, 1000, 2000, 2000};
  uint32_t decayLengths  [4] = {1000, 2000, 4000, 4000};
  uint8_t  maxLightLevels[4] = { 100,  150,  200,  100};
  uint32_t tauInMillisec [4] = { 250,  500, 1000,    0};
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 4);

  
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

  DecayLight light1;
  light1.setup(lightVariable,
               onLightLevelValue,
               numIntervals,
               onLengths,
               decayLengths,
               maxLightLevels,
               tauInMillisec);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  
  uint32_t timeMS = 0;
  uint8_t i;
  uint8_t j;
  for (i = 0; i < intervals; i++) {
    j = i % sizeof(decaying)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);
    light1.update();
    // std::cerr << "i, j, millis, decaying, lightLevel = "
    //           << int(i) << ", " << int(j) << ", "
    //           << int(timeMS) << ", " << light1.decaying << ", "  << int(light1())
    //           << std::endl;
    EXPECT_EQ(level[j]   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(decaying[j], light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeSteps[j];
  }

  light1.on();

  uint8_t expectedLevel;

  for (i = 0; i < intervals; i++) {
    j = i % sizeof(decaying)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);

    if (i == 18)
      light1.resume();

    if (i < 18) {
      expectedLevel    = onLightLevelValue;
    } else {
      expectedLevel    = level[j];
    }
    
    light1.update();

    EXPECT_EQ(expectedLevel   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(decaying[j], light1.decaying)
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
    .Times(2*intervals);
  
  uint8_t lightVariable;


  uint32_t onLengths     [4] = { 500, 1000, 2000, 2000};
  uint32_t decayLengths  [4] = {1000, 2000, 4000, 4000};
  uint8_t  maxLightLevels[4] = { 100,  150,  200,  100};
  uint32_t * tauInMillisec = NULL;
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 4);

  
  DecayLight light1;
  light1.setup(lightVariable,
               onLightLevelValue,
               numIntervals,
               onLengths,
               decayLengths,
               maxLightLevels,
               tauInMillisec);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);

  // Expected Values:
  // See table above
 
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

  for (i = 0; i < intervals; i++) {
    j = i % sizeof(decaying)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);

    if (i == 18)
      light1.resume();

    if (i < 18) {
      expectedLevel    = onLightLevelValue;
    } else {
      expectedLevel    = level[j];
    }
    
    light1.update();
    EXPECT_EQ(expectedLevel   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(decaying[j], light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeSteps[j];
    j++;
  }
  

  releaseArduinoMock();
}

TEST(DecayLight, UpdateCalledInfrequently) {
  const uint8_t intervals = 3*24;
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(intervals);
  
  uint8_t lightVariable;

  // Expected Values:

  //  i   j   mSec   decaying  lightLevel  decayStartTime  changeTime  intervalIndex
  // --  --  ------  --------  ----------  --------------  ----------  -------------
     
  //  0   0       0         0         150               0        1000              1
  //  1   1     250         0         150               0        1000              1
  //  2   2     500         0         150               0        1000              1
  // decayStartTime, changeTime, decaying = 10500, 12500, 1
  //  3   3   10500         1         150           10500       12500              1
  //  4   4   11500         1          20           10500       12500              1
  //  5   5   12000         1           7           10500       12500              1
  // decayStartTime, changeTime, decaying = 22000, 24000, 0
     
  //  6   6   22000         0         200           22000       24000              2
  //  7   7   22500         0         200           22000       24000              2
  //  8   8   23000         0         200           22000       24000              2
  // decayStartTime, changeTime, decaying = 33000, 37000, 1
  //  9   9   33000         1         200           33000       37000              2
  // 10  10   35000         1          27           33000       37000              2
  // 11  11   36000         1          10           33000       37000              2
  // decayStartTime, changeTime, decaying = 46000, 48000, 0
     
  // 12  12   46000         0         100           46000       48000              3
  // 13  13   46500         0         100           46000       48000              3
  // 14  14   47000         0         100           46000       48000              3
  // decayStartTime, changeTime, decaying = 57000, 61000, 1
  // 15  15   57000         1           0           57000       61000              3
  // 16  16   59000         1           0           57000       61000              3
  // 17  17   60000         1           0           57000       61000              3
  // decayStartTime, changeTime, decaying = 70000, 70500, 0
     
  // 18  18   70000         0         100           70000       70500              4
  // 19  19   70100         0         100           70000       70500              4
  // 20  20   70250         0         100           70000       70500              4
  // decayStartTime, changeTime, decaying = 80250, 81250, 1
  // 21  21   80250         1         100           80250       81250              4
  // 22  22   80500         1          37           80250       81250              4
  // 23  23   80750         1          14           80250       81250              4
  // decayStartTime, changeTime, decaying = 90750, 91750, 0
 
  // NOTE: Becuase of the way DecayLight::update() does its indexing
  // the arrays passed to the constructor the when update() is called
  // for the frist time the array indexing is
  // 1..numIntervals-1
  // 0..numIntervals-1
  // 0..numIntervals-1
  // ...
  // That is why the expected values are off by one

  //                                   ---------------- Starts here
  //                                  \/
  uint32_t onLengths     [4] = { 500, 1000, 2000, 2000};
  uint32_t decayLengths  [4] = {1000, 2000, 4000, 4000};
  uint8_t  maxLightLevels[4] = { 100,  150,  200,  100};
  uint32_t tauInMillisec [4] = { 250,  500, 1000,    0};
  const uint8_t  numIntervals = sizeof(maxLightLevels)/sizeof(uint8_t);
  assert (numIntervals == 4);

  //                                On   Decay  Decay  Decay Decay Decay
  const uint16_t  timeSteps[4*6] = {250,  250,  10000, 1000,  500, 10000,   // 2500, 
                                    500,  500,  10000, 2000, 1000, 10000,   // 5000, 
                                    500,  500,  10000, 2000, 1000, 10000,   // 5000, 
                                    100,  150,  10000,  250,  250, 10000  };// 1250, 
  uint8_t  level[4*6] = {150, 150, 150, 150, 20,  7,
                         200, 200, 200, 200, 27, 10, 
                         100, 100, 100, OFF,OFF,OFF,
                         100, 100, 100, 100, 37, 14 };
                                  
  bool     decaying[4*6] = {false, false, false, true, true, true,
                            false, false, false, true, true, true,
                            false, false, false, true, true, true,
                            false, false, false, true, true, true };

  DecayLight light1;
  light1.setup(lightVariable,
               onLightLevelValue,
               numIntervals,
               onLengths,
               decayLengths,
               maxLightLevels,
               tauInMillisec);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  
  uint32_t timeMS = 0;
  uint8_t i;
  uint8_t j;
  for (i = 0; i < intervals; i++) {
    j = i % sizeof(decaying)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);
    light1.update();
    // if (i == 0) {
    //   std::cerr << " i   j   mSec   decaying  lightLevel  decayStartTime  changeTime  intervalIndex" << std::endl;
    //   std::cerr << "--  --  ------  --------  ----------  --------------  ----------  -------------" << std::endl;
    // }
    // if (j % 6 == 0) std::cerr << std::endl;
    // std::cerr << setfill(' ') << setw(2) << int(i)
    //           << setfill(' ') << setw(4) << int(j)
    //           << setfill(' ') << setw(8) << int(timeMS)
    //           << setfill(' ') << setw(10) << light1.decaying
    //           << setfill(' ') << setw(12) << int(light1())
    //           << setfill(' ') << setw(16) << int(light1.decayStartTime)
    //           << setfill(' ') << setw(12) << int(light1.changeTime)
    //           << setfill(' ') << setw(15) << int(light1.intervalIndex)
    //           << std::endl;
    EXPECT_EQ(level[j]   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(decaying[j], light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeSteps[j];
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
  
  FlashingLight light1;
  light1.setup(lightVariable,
               onLightLevelValue,
               numIntervals,
               onLengths,
               decayLengths,
               maxLightLevels);
  
  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF  , *light1.p_lightLevel);
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

  BlinkingLight light1;
  light1.setup(lightVariable,
               onLightLevelValue,
               1000, // onLengthValue
               10  , // offLengthValue
               maxLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(1000, light1.onLength[0]);
  EXPECT_EQ(10  , light1.decayLength[0]);
  EXPECT_EQ(maxLightLevelValue , light1.maxLightLevel[0]);
}

TEST(BlinkingLight, Update)
{
  const uint8_t intervals = 3*24;
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(2*intervals);
  
  uint8_t lightVariable;


  uint32_t onLength      =  500;
  uint32_t offLength     = 1000; 
  uint8_t  maxLightLevel =  100; 
  
  BlinkingLight light1;
  light1.setup(lightVariable,
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

  for (i = 0; i < intervals; i++) {
    j = i % sizeof(off)/sizeof(bool);
    arduinoMock->setMillisRaw(timeMS);
    
    if (i == 18)
      light1.resume();
    
    if (i < 18) {
      expectedLevel    = onLightLevelValue;
    } else {
      expectedLevel    = level[j];
    }

    light1.update();
    EXPECT_EQ(expectedLevel   , light1())
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    EXPECT_EQ(off[j], light1.decaying)
      << "i,j = " << int(i) << ", " << int(j) << std::endl;
    
    timeMS += timeStep;
  }
  
  
  releaseArduinoMock();
}



TEST(FastBlinkingLight, Constructor)
{
  uint8_t lightVariable;

  FastBlinkingLight light1;
  light1.setup(lightVariable, onLightLevelValue, maxLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(fastOnLengthValue , light1.onLength[0]);
  EXPECT_EQ(fastOffLengthValue, light1.decayLength[0]);
  EXPECT_EQ(maxLightLevelValue, light1.maxLightLevel[0]);
}

TEST(SlowBlinkingLight, Constructor)
{
  uint8_t lightVariable;

  SlowBlinkingLight light1;
  light1.setup(lightVariable, onLightLevelValue,
               maxLightLevelValue);

  EXPECT_EQ(onLightLevelValue, light1.onLightLevel);
  EXPECT_EQ(OFF, *light1.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.getPaused());

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(slowOnLengthValue , light1.onLength[0]);
  EXPECT_EQ(slowOffLengthValue, light1.decayLength[0]);
  EXPECT_EQ(maxLightLevelValue, light1.maxLightLevel[0]);
}

TEST(FastSlowBlinkingLight, Constructor)
{
  uint8_t lightVariable;

  FastSlowBlinkingLight light1;
  EXPECT_EQ(FastSlowBlinkingLight::NOTSET , light1.getSpeed());
  EXPECT_EQ(NULL, light1.fastLight.p_lightLevel);
  EXPECT_EQ(NULL, light1.slowLight.p_lightLevel);
  EXPECT_EQ(NULL, light1.p_currentLight);
  
  light1.setup(lightVariable, onLightLevelValue, maxLightLevelValue);
  
  EXPECT_EQ(onLightLevelValue, light1.fastLight.onLightLevel);
  EXPECT_EQ(OFF, *light1.fastLight.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.fastLight.getPaused());
  
  EXPECT_EQ(onLightLevelValue, light1.slowLight.onLightLevel);
  EXPECT_EQ(OFF, *light1.slowLight.p_lightLevel);
  EXPECT_EQ(OFF, lightVariable);
  EXPECT_EQ(false, light1.slowLight.getPaused());
  
  EXPECT_EQ(0,    light1.fastLight.changeTime);
  EXPECT_EQ(fastOnLengthValue , light1.fastLight.onLength[0]);
  EXPECT_EQ(fastOffLengthValue, light1.fastLight.decayLength[0]);
  EXPECT_EQ(maxLightLevelValue, light1.fastLight.maxLightLevel[0]);
  
  EXPECT_EQ(0,    light1.slowLight.changeTime);
  EXPECT_EQ(slowOnLengthValue , light1.slowLight.onLength[0]);
  EXPECT_EQ(slowOffLengthValue, light1.slowLight.decayLength[0]);
  EXPECT_EQ(maxLightLevelValue, light1.slowLight.maxLightLevel[0]);
}

TEST(FastSlowBlinkingLight, SetToFast)
{
  uint8_t lightVariable;
  
  FastSlowBlinkingLight light1;
  EXPECT_EQ(FastSlowBlinkingLight::NOTSET , light1.getSpeed());
  
  light1.setup(lightVariable, onLightLevelValue, maxLightLevelValue);
  
  light1.setToFast();
  
  EXPECT_EQ(0,    light1.p_currentLight->changeTime);
  EXPECT_EQ(fastOnLengthValue , light1.p_currentLight->onLength[0]);
  EXPECT_EQ(fastOffLengthValue, light1.p_currentLight->decayLength[0]);
  EXPECT_EQ(maxLightLevelValue, light1.p_currentLight->maxLightLevel[0]);
  EXPECT_EQ(FastSlowBlinkingLight::FAST , light1.getSpeed());
}

TEST(FastSlowBlinkingLight, SetToSlow)
{
  uint8_t lightVariable;
  
  FastSlowBlinkingLight light1;
  EXPECT_EQ(FastSlowBlinkingLight::NOTSET , light1.getSpeed());
  
  light1.setup(lightVariable, onLightLevelValue, maxLightLevelValue);
  
  light1.setToSlow();
  
  EXPECT_EQ(0,    light1.p_currentLight->changeTime);
  EXPECT_EQ(slowOnLengthValue , light1.p_currentLight->onLength[0]);
  EXPECT_EQ(slowOffLengthValue, light1.p_currentLight->decayLength[0]);
  EXPECT_EQ(maxLightLevelValue, light1.p_currentLight->maxLightLevel[0]);
  EXPECT_EQ(FastSlowBlinkingLight::SLOW , light1.getSpeed());
}

TEST(FastSlowBlinkingLight, FunctionCallOperatorGetValue) {

  uint8_t lightVariable;

  FastSlowBlinkingLight light1;
  light1.setup(lightVariable, onLightLevelValue,
                               maxLightLevelValue);

  light1.setToSlow();

  EXPECT_EQ(onLightLevelValue, light1.p_currentLight->onLightLevel);
  *light1.p_currentLight->p_lightLevel = ON;
  EXPECT_EQ(ON, light1());
  EXPECT_EQ(ON, lightVariable);

  *light1.p_currentLight->p_lightLevel = OFF;
  EXPECT_EQ(OFF, light1());
  EXPECT_EQ(OFF, lightVariable);

  for (uint8_t i = 0; i < OFF; i++) {
    *light1.p_currentLight->p_lightLevel = i;
    EXPECT_EQ(i, light1());
    EXPECT_EQ(i, lightVariable);
  }
    
}
