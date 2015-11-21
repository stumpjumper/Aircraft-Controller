#include "lucky7.h"
#include <gtest/gtest.h>

TEST(TimeOfDayTest, Constructor) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0};
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

TEST(TimeOfDayTest, SetsAndGets) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0};
  arduinoMock->setMillis(millisTimesArray);

  TimeOfDay tod = TimeOfDay(0,1000,10);

  tod.setPhotocellAvgValueCurrent(991);
  tod.setPhotocellAvgValueMin(992);
  tod.setPhotocellAvgValueMax(993);
  tod.setNightDayThresholdPercentage(22);
  tod.setLenghtOfNight(994);
  tod.setNightStart(995);
  tod.setDayStart(996);
  tod.setUpdate30secTimeout(997);
  tod.setUpdate5minTimeout(998);
  tod.setEveningLength(999);
  tod.setMorningLength(1000);
  tod.setPredawnLength(1001);
  tod.setPhotocellValuesIndex(12);
  tod.setCurrentDayPart(TimeOfDay::EVENING);
  
  // Test the TimeOfDay class's constructor
  EXPECT_EQ(991, tod.getPhotocellAvgValueCurrent());
  EXPECT_EQ(992, tod.getPhotocellAvgValueMin());
  EXPECT_EQ(993, tod.getPhotocellAvgValueMax());
  EXPECT_EQ(22, tod.getNightDayThresholdPercentage());
  EXPECT_EQ(994, tod.getLenghtOfNight());
  EXPECT_EQ(995, tod.getNightStart());
  EXPECT_EQ(996, tod.getDayStart());
  EXPECT_EQ(997, tod.getUpdate30secTimeout());
  EXPECT_EQ(998, tod.getUpdate5minTimeout());
  EXPECT_EQ(999, tod.getEveningLength());
  EXPECT_EQ(1000, tod.getMorningLength());
  EXPECT_EQ(12, tod.getPhotocellValuesIndex());
  EXPECT_EQ(TimeOfDay::EVENING, tod.getDayPart());

  releaseArduinoMock();

}

TEST(TimeOfDayTest, getNightDayThreshold) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0};
  arduinoMock->setMillis(millisTimesArray);

  TimeOfDay tod = TimeOfDay(0,1000,10);

  tod.setPhotocellAvgValueMin(0);
  tod.setPhotocellAvgValueMax(1000);
  tod.setNightDayThresholdPercentage(22);
  EXPECT_EQ(220, tod.getNightDayThreshold());

  tod.setPhotocellAvgValueMin(11);
  tod.setPhotocellAvgValueMax(1010);
  tod.setNightDayThresholdPercentage(22);
  EXPECT_EQ(230, tod.getNightDayThreshold());

  tod.setPhotocellAvgValueMin(10);
  tod.setPhotocellAvgValueMax(1015);
  tod.setNightDayThresholdPercentage(22);
  EXPECT_EQ(231, tod.getNightDayThreshold());

  
  releaseArduinoMock();
}

TEST(TimeOfDayTest, UpdateValuesAndTimeOfDay) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0};
  arduinoMock->setMillis(millisTimesArray);

  TimeOfDay tod = TimeOfDay(500,501,10);

  tod.updateValuesAndTimeOfDay(898);
  tod.updateValuesAndTimeOfDay(900);
  tod.updateValuesAndTimeOfDay(999);
  tod.updateValuesAndTimeOfDay(410);
  tod.updateValuesAndTimeOfDay(323);
  tod.updateValuesAndTimeOfDay(281);
  tod.updateValuesAndTimeOfDay(102);
  tod.updateValuesAndTimeOfDay(13);

  EXPECT_EQ(13, tod.getPhotocellAvgValueMin());
  EXPECT_EQ(999, tod.getPhotocellAvgValueMax());
  
  releaseArduinoMock();
}




