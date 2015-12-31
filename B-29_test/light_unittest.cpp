#include "lucky7.h"
#include <gtest/gtest.h>

TEST(OnOffLight, Constructor) {

  OnOffLight light1;

  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);
}

TEST(OnOffLight, On) {

  OnOffLight light1;

  EXPECT_EQ(false, light1.paused);

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.on();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(true, light1.paused);
}

TEST(OnOffLight, Off) {

  OnOffLight light1;

  EXPECT_EQ(false, light1.paused);

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.off();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(true, light1.paused);
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

  EXPECT_EQ(false, light1.paused);

  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.paused);

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);

  light1.paused = true;
  EXPECT_EQ(true, light1.paused);

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.paused);

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);

  light1.paused = false;
  EXPECT_EQ(false, light1.paused);

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.resume();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);
}

TEST(OnOffLight, Update) {

  OnOffLight light1;

  EXPECT_EQ(false, light1.paused);


  // paused = true case
  light1.paused = true;
  EXPECT_EQ(true, light1.paused);

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.update();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(true, light1.paused);

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.update();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(true, light1.paused);

  // paused = false case
  light1.paused = false;
  EXPECT_EQ(false, light1.paused);

  light1.lightLevel = OFF;
  EXPECT_EQ(OFF, light1.lightLevel);
  light1.update();
  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);

  light1.lightLevel = ON;
  EXPECT_EQ(ON, light1.lightLevel);
  light1.update();
  EXPECT_EQ(ON, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);
}


TEST(BlinkingLight, Constructor)
{
  BlinkingLight light1(1000, // onLengthValue
                       10  , // offLengthValue
                       199); // maxLightLevelVaue

  EXPECT_EQ(OFF, light1.lightLevel);
  EXPECT_EQ(false, light1.paused);

  EXPECT_EQ(0,    light1.changeTime);
  EXPECT_EQ(1000, light1.onLength);
  EXPECT_EQ(10  , light1.offLength);
  EXPECT_EQ(199 , light1.maxLightLevel);
}


// TEST(BlinkingLight, Update)
// {
//   BlinkingLight light1(1000, // onLengthValue
//                        10  , // offLengthValue
//                        199); // maxLightLevelVaue

//   EXPECT_EQ(OFF, light1.lightLevel);
//   EXPECT_EQ(false, light1.paused);

//   EXPECT_EQ(0,    light1.changeTime);
//   EXPECT_EQ(1000, light1.onLength);
//   EXPECT_EQ(10  , light1.offLength);
//   EXPECT_EQ(199 , light1.maxLightLevel);
// }
