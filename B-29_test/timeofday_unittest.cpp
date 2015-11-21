#include "lucky7.h"
#include <gtest/gtest.h>

TEST(TimeOfDayTest, Constructor) {
  TimeOfDay tod = TimeOfDay(0,1000,10);
  // Test the TimeOfDay class's constructor
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
}

TEST(TimeOfDayTest, getNightDayThreshold) {
  TimeOfDay tod = TimeOfDay(0,1000,10);

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
}

TEST(TimeOfDayTest, UpdatePhotocellAvgValues) {
  TimeOfDay tod = TimeOfDay(500,501,10);

  tod.updatePhotocellAvgValues(898);
  tod.updatePhotocellAvgValues(900);
  tod.updatePhotocellAvgValues(999);
  tod.updatePhotocellAvgValues(410);
  tod.updatePhotocellAvgValues(323);
  tod.updatePhotocellAvgValues(281);
  tod.updatePhotocellAvgValues(102);
  tod.updatePhotocellAvgValues(13);
  tod.updatePhotocellAvgValues(824);

  EXPECT_EQ(13, tod.getPhotocellAvgValueMin());
  EXPECT_EQ(999, tod.getPhotocellAvgValueMax());
  EXPECT_EQ(824, tod.getPhotocellAvgValueCurrent());
}

TEST(TimeOfDayTest, UpdateTimeOfDay) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0};
  arduinoMock->setMillis(millisTimesArray);

  TimeOfDay tod = TimeOfDay(500,501,10);

  releaseArduinoMock();
}




