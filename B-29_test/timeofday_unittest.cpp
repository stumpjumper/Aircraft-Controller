#include "lucky7.h"
#include <gtest/gtest.h>

TEST(TimeOfDayTest, Constructor) {
  ArduinoMock* arduinoMock = arduinoMockInstance();

  unsigned long millisTimesArray[] = {0,10,20,30,40};
  arduinoMock->setMillis(millisTimesArray);

  TimeOfDay tod = TimeOfDay(0,1000,10);
  // Test the TimeOfDay class's constructor
  EXPECT_EQ(LUCKY7_TIME30SEC, tod.getUpdate30secTimeout());

  releaseArduinoMock();
}
