#include "lucky7.h"
#include <gtest/gtest.h>

TEST(TimeOfDayTest, setup) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillOnce(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(0,1000,10);

  // Test TimeOfDay.setup()
  EXPECT_EQ(10, tod.nightDayThresholdPercentage);
  EXPECT_EQ(0, tod.photocellAvgValueMin);
  EXPECT_EQ(1000, tod.getPhotocellAvgValueMax());
  EXPECT_EQ(0, tod.getPhotocellAvgValueCurrent());

  EXPECT_EQ(LUCKY7_TIME30SEC, tod.update30secTimeout);
  EXPECT_EQ(LUCKY7_TIME5MIN, tod.update5minTimeout);
  EXPECT_EQ(LUCKY7_TIME4HOUR, tod.eveningLength);
  EXPECT_EQ(LUCKY7_TIME2HOUR, tod.morningLength);
  EXPECT_EQ(0, tod.photocellValuesIndex);

  EXPECT_EQ(LUCKY7_TIME12HOUR, tod.lenghtOfNight);
  EXPECT_EQ(0, tod.nightStart);
  EXPECT_EQ(0, tod.dayStart);
  EXPECT_EQ(TimeOfDay::DAY, tod.getDayPart());

  releaseArduinoMock();
}

TEST(TimeOfDayTest, getNightDayThreshold) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillOnce(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(0,1000,10);

  tod.photocellAvgValueMin = 0;
  tod.photocellAvgValueMax = 1000;
  tod.nightDayThresholdPercentage = 22;
  EXPECT_EQ(220, tod.getNightDayThreshold());

  tod.photocellAvgValueMin = 11;
  tod.photocellAvgValueMax = 1010;
  tod.nightDayThresholdPercentage = 22;
  EXPECT_EQ(230, tod.getNightDayThreshold());

  tod.photocellAvgValueMin = 10;
  tod.photocellAvgValueMax = 1015;
  tod.nightDayThresholdPercentage = 22;
  EXPECT_EQ(231, tod.getNightDayThreshold());

  releaseArduinoMock();
}

TEST(TimeOfDayTest, UpdatePhotocellAvgValues) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillOnce(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(500,501,10);

  uint16_t values   [10] = {801, 900, 950,1000, 410, 323, 281, 102,  13, 824};
  uint16_t valuesMin[10] = {500, 500, 500, 500, 410, 323, 281, 102,  13,  13};
  uint16_t valuesMax[10] = {801, 900, 950,1000,1000,1000,1000,1000,1000,1000};
    

  for (uint8_t i = 0; i < 10; i++) {
    tod.updatePhotocellAvgValues(values[i]);
    EXPECT_EQ(values   [i], tod.getPhotocellAvgValueCurrent());
    EXPECT_EQ(valuesMin[i], tod.getPhotocellAvgValueMin    ());
    EXPECT_EQ(valuesMax[i], tod.getPhotocellAvgValueMax    ());
  }

  releaseArduinoMock();
}

TEST(TimeOfDayTest, UpdateAverage) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillRepeatedly(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(100,250,10);

  uint16_t * lightLevel;
  uint16_t   lightLevelArray[4][10] =
    {{ 10, 100,  30,  70,  50,  80,  40,  60,  90,  20},   // Avg = 55 (Note: Drop high and low)
     {  5,  95,  25,  65,  45,  75,  35,  55,  85,  15},   // Avg = 50
     { 50, 950, 250, 650, 450, 750, 350, 550, 850, 150},   // Avg = 500
     {100,1000, 300, 700, 500, 800, 400, 600, 900, 200} }; // Avg = 550
  uint32_t   avgValue   [4] = { 55,  50, 500, 550};
  uint32_t   avgValueMin[4] = { 55,  50,  50,  50};
  uint32_t   avgValueMax[4] = {250, 250, 500, 550};
  uint16_t   lightLevelIgnore = 10000;
  uint32_t   millisIncr = 15;

  arduinoMock->setMillisRaw(10);

  for (uint8_t j = 0; j < 4; j++) {
    lightLevel = lightLevelArray[j];
    for (uint8_t i = 0; i < 10; i++) {
      arduinoMock->addMillisSecs(millisIncr);
      tod.updateAverage(lightLevelIgnore);
      
      arduinoMock->addMillisSecs(millisIncr);
      tod.updateAverage(lightLevel[i]);
    }
    EXPECT_EQ(avgValue   [j], tod.getPhotocellAvgValueCurrent());
    EXPECT_EQ(avgValueMin[j], tod.getPhotocellAvgValueMin());
    EXPECT_EQ(avgValueMax[j], tod.getPhotocellAvgValueMax());
  }

  releaseArduinoMock();
}

TEST(TimeOfDayTest, UpdateTimeOfDay) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillOnce(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(500,501,10);

  releaseArduinoMock();
}




