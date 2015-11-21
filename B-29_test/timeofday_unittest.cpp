#include "lucky7.h"
#include <gtest/gtest.h>

TEST(TimeOfDayTest, Constructor) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0,10,20,30,40};
  arduinoMock->setMillis(millisTimesArray);

  TimeOfDay tod = TimeOfDay(0,1000,10);
  // Test the TimeOfDay class's constructor
  EXPECT_EQ(10, tod.getNightDayThresholdPercentage());
  EXPECT_EQ(0, tod.getPhotocellAvgValueMin());
  EXPECT_EQ(1000, tod.getPhotocellAvgValueMax());
  EXPECT_EQ(0, tod.getPhotocellAvgValueCurrent());

  EXPECT_EQ(LUCKY7_TIME30SEC, tod.getUpdate30secTimeout());
  EXPECT_EQ(LUCKY7_TIME5MIN, tod.getUpdate5minTimeout());
  EXPECT_EQ(LUCKY7_TIME4HOUR, tod.getEveningLength());
  EXPECT_EQ(LUCKY7_TIME2HOUR, tod.getMorningLength());
  EXPECT_EQ(0, tod.getPhotocellValuesIndex());

  EXPECT_EQ(LUCKY7_TIME12HOUR, tod.getLenghtOfNight());
  EXPECT_EQ(0, tod.getNightStart());
  EXPECT_EQ(0, tod.getDayStart());
  EXPECT_EQ(TimeOfDay::DAY, tod.getDayPart());

  releaseArduinoMock();
}
