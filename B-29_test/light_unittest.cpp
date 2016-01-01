#include "lucky7.h"
#include <gtest/gtest.h>

TEST(OnOffLight, Constructor) {

  OnOffLight light1;

  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);
}

TEST(OnOffLight, On) {

  OnOffLight light1;

  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.on();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(true, light1.getPaused());
}

TEST(OnOffLight, Off) {

  OnOffLight light1;

  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.off();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(true, light1.getPaused());
}

TEST(OnOffLight, FunctionCallOperatorGetValue) {

  OnOffLight light1;

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1());

  for (uint8_t i = 0; i < OFF; i++) {
    light1.lightLevel = i;
    EXPECT_EQ(i, light1());
  }
    
}

TEST(OnOffLight, FunctionCallOperatorSetValue) {

  OnOffLight light1;

  light1() = ON;
  EXPECT_EQ(ON, light1.lightLevel);

  light1() = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);

  for (uint8_t i = 0; i < OFF; i++) {
    light1() = i;
    EXPECT_EQ(i, light1.lightLevel);
  }
    
}

TEST(OnOffLight, Resume) {

  OnOffLight light1;

  EXPECT_EQ(false, light1.getPaused());

  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.getPaused());

  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(false, light1.getPaused());

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.getPaused());

  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(false, light1.getPaused());
}

TEST(OnOffLight, Update) {

  OnOffLight light1;

  EXPECT_EQ(false, light1.getPaused());


  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.update();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(true, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.update();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(true, light1.getPaused());

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.update();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.getPaused());

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.update();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(false, light1.getPaused());
}


TEST(BlinkingLight, Constructor)
{
  BlinkingLight light1(1000, // onLengthValue
                       10  , // offLengthValue
                       199); // maxLightLevelVaue

  EXPECT_EQ(OFF, light1.lightLevel);
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

  uint32_t onLengthValue  = 1000; // On for 1 second
  uint32_t offLengthValue =  500; // Off .5 seconds
  uint8_t  maxLightLevel  =  199; 

  BlinkingLight light1(onLengthValue, offLengthValue, maxLightLevel);

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
  }
  
  releaseArduinoMock();
}
