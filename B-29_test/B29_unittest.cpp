#include "B29_unittest.h"
#include "B-29.ino"
#include <gtest/gtest.h>
#include "IRremoteMock.h"

using testing::AtLeast;

TEST_F(B29Test, ArduinoMockMillis) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillRepeatedly(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  unsigned long currentValue = 0;
  unsigned long addValue = 0;
  

  EXPECT_EQ(0, millis());

  currentValue = 0;
  arduinoMock->setMillisRaw(currentValue);
  EXPECT_EQ(currentValue, millis());

  currentValue = 9;
  arduinoMock->setMillisRaw(currentValue);
  EXPECT_EQ(currentValue, millis());

  currentValue = 11;
  arduinoMock->setMillisSecs(currentValue);
  EXPECT_EQ(currentValue*1000, millis());

  currentValue = 21;
  arduinoMock->setMillisMins(currentValue);
  EXPECT_EQ(currentValue*60*1000, millis());

  currentValue = 31;
  arduinoMock->setMillisHrs(currentValue);
  EXPECT_EQ(currentValue*60*60*1000, millis());

  currentValue = 0;
  arduinoMock->setMillisRaw(0);

  addValue = 11;
  currentValue += addValue*1000;
  arduinoMock->addMillisSecs(addValue);
  EXPECT_EQ(currentValue, millis());

  addValue = 21;
  currentValue += addValue*60*1000;
  arduinoMock->addMillisMins(addValue);
  EXPECT_EQ(currentValue, millis());

  addValue = 31;
  currentValue += addValue*60*60*1000;
  arduinoMock->addMillisHrs(addValue);
  EXPECT_EQ(currentValue, millis());

  //----------- Test when hours is a float
  float addHrs = 4.5;
  currentValue += 4*60*60*1000 + 30*60*1000;
  arduinoMock->addMillisHrs(addHrs);
  EXPECT_EQ(currentValue, millis());

  addHrs = .25;
  currentValue += 60*60*250;
  arduinoMock->addMillisHrs(addHrs);
  EXPECT_EQ(currentValue, millis());

  addHrs = .1;
  currentValue += 60*60*100;
  arduinoMock->addMillisHrs(addHrs);
  EXPECT_EQ(currentValue, millis());

  releaseArduinoMock();
}

TEST_F(B29Test, ResetTimeoutBatteryLow) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillRepeatedly(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  arduinoMock->setMillisRaw(0);
  resetTimeoutBatteryLow();
  EXPECT_EQ(0+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  arduinoMock->setMillisRaw(10);
  resetTimeoutBatteryLow();
  EXPECT_EQ(10+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  arduinoMock->setMillisRaw(20);
  resetTimeoutBatteryLow();
  EXPECT_EQ(20+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  arduinoMock->setMillisRaw(30);
  resetTimeoutBatteryLow();
  EXPECT_EQ(30+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  releaseArduinoMock();
}

TEST_F(B29Test, HardWareSetup) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));

  IRrecvMock & irrecv = getIRrecv();
  EXPECT_CALL(irrecv, enableIRIn())
    .Times(AtLeast(1));
  
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


TEST_F(B29Test, OverrideBatteryLow) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));

  IRrecvMock & irrecv = getIRrecv();
  EXPECT_CALL(irrecv, enableIRIn())
    .Times(AtLeast(1));

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

TEST_F(B29Test, ResetTimeoutOverride) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillRepeatedly(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  arduinoMock->setMillisRaw(0);
  resetTimeoutOverride();
  EXPECT_EQ(0+TIMEOUTOVERRIDE, timeoutOverride);

  arduinoMock->setMillisRaw(10);
  resetTimeoutOverride();
  EXPECT_EQ(10+TIMEOUTOVERRIDE, timeoutOverride);

  arduinoMock->setMillisRaw(20);
  resetTimeoutOverride();
  EXPECT_EQ(20+TIMEOUTOVERRIDE, timeoutOverride);

  arduinoMock->setMillisRaw(30);
  resetTimeoutOverride();
  EXPECT_EQ(30+TIMEOUTOVERRIDE, timeoutOverride);

  releaseArduinoMock();
}

TEST_F(B29Test, NameLightGroupsOnAndOff) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));
  
  IRrecvMock & irrecv = getIRrecv();
  EXPECT_CALL(irrecv, enableIRIn())
    .Times(AtLeast(1));

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

TEST_F(B29Test, AllLightsOnAndOff) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(testing::_,testing::_))
    .Times(testing::AtLeast(1));
  
  IRrecvMock & irrecv = getIRrecv();
  EXPECT_CALL(irrecv, enableIRIn())
    .Times(AtLeast(1));

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


