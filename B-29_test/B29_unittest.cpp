#include "B-29.ino"
#include <gtest/gtest.h>

TEST(B29Test, ResetTimeoutBatteryLow) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0,10,20,30};
  arduinoMock->setMillis(millisTimesArray);

  resetTimeoutBatteryLow();
  EXPECT_EQ(0+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  resetTimeoutBatteryLow();
  EXPECT_EQ(10+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  resetTimeoutBatteryLow();
  EXPECT_EQ(20+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  resetTimeoutBatteryLow();
  EXPECT_EQ(30+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  releaseArduinoMock();
}

TEST(B29Test, HardWareSetup) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));
  
  hw.setup();
  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o7 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);

  releaseArduinoMock();
}


TEST(B29Test, OverrideBatteryLow) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));
  
  hw.setup();

  hw.o3 = ON;
  hw.o7 = ON;
  EXPECT_EQ(true, overrideBatteryLow());

  hw.o3 = ON;
  hw.o7 = OFF;
  EXPECT_EQ(true, overrideBatteryLow());

  hw.o3 = OFF;
  hw.o7 = ON;
  EXPECT_EQ(true, overrideBatteryLow());

  hw.o3 = OFF;
  hw.o7 = OFF;
  EXPECT_EQ(false, overrideBatteryLow());
  
  releaseArduinoMock();
}

TEST(B29Test, ResetTimeoutOverride) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0,10,20,30};
  arduinoMock->setMillis(millisTimesArray);

  resetTimeoutOverride();
  EXPECT_EQ(0+TIMEOUTOVERRIDE, timeoutOverride);

  resetTimeoutOverride();
  EXPECT_EQ(10+TIMEOUTOVERRIDE, timeoutOverride);

  resetTimeoutOverride();
  EXPECT_EQ(20+TIMEOUTOVERRIDE, timeoutOverride);

  resetTimeoutOverride();
  EXPECT_EQ(30+TIMEOUTOVERRIDE, timeoutOverride);

  releaseArduinoMock();
}

TEST(B29Test, NameLightGroupsOnAndOff) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));
  
  hw.setup();

  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o7 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);

  idenOn();
  EXPECT_EQ(ON, hw.o1 );
  landOn();
  EXPECT_EQ(ON, hw.o2 );
  illuOn();
  EXPECT_EQ(ON, hw.o4 );
  posiOn();
  EXPECT_EQ(ON, hw.o5 );
  formOn();
  EXPECT_EQ(ON, hw.o6 );
  blueOn();
  EXPECT_EQ(ON, hw.o8 );
  redOn ();
  EXPECT_EQ(ON, hw.o13 );

  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  idenOff();
  EXPECT_EQ(OFF, hw.o1 );
  landOff();
  EXPECT_EQ(OFF, hw.o2 );
  illuOff();
  EXPECT_EQ(OFF, hw.o4 );
  posiOff();
  EXPECT_EQ(OFF, hw.o5 );
  formOff();
  EXPECT_EQ(OFF, hw.o6 );
  blueOff();
  EXPECT_EQ(OFF, hw.o8 );
  redOff ();
  EXPECT_EQ(OFF, hw.o13 );

  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  releaseArduinoMock();
}

TEST(B29Test, AllLightsOnAndOff) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));
  
  hw.setup();

  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o7 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);

  allLightsOn();
  EXPECT_EQ(ON, hw.o1 );
  EXPECT_EQ(ON, hw.o2 );
  EXPECT_EQ(ON, hw.o4 );
  EXPECT_EQ(ON, hw.o5 );
  EXPECT_EQ(ON, hw.o6 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  allLightsOff();
  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  releaseArduinoMock();
}


