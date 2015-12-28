#include "lucky7.h"
#include "pins_arduino.h"
#include <gtest/gtest.h>

using ::testing::AnyOf;

TEST(Lucky7Test, Setup) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(AnyOf(O1,O2,O3,O4,O5,O6,O7,O8,O13),OUTPUT))
    .Times(9);

  EXPECT_CALL(*arduinoMock, pinMode(AnyOf(A4,A5),INPUT))
    .Times(2);

  Lucky7 lucky7 = Lucky7();
  lucky7.setup();

  EXPECT_EQ(0, lucky7.o1 );
  EXPECT_EQ(0, lucky7.o2 );
  EXPECT_EQ(0, lucky7.o3 );
  EXPECT_EQ(0, lucky7.o4 );
  EXPECT_EQ(0, lucky7.o5 );
  EXPECT_EQ(0, lucky7.o6 );
  EXPECT_EQ(0, lucky7.o7 );
  EXPECT_EQ(0, lucky7.o8 );
  EXPECT_EQ(0, lucky7.o13);

  EXPECT_EQ(0, lucky7.o1Saved);
  EXPECT_EQ(0, lucky7.o2Saved);
  EXPECT_EQ(0, lucky7.o3Saved);
  EXPECT_EQ(0, lucky7.o4Saved);
  EXPECT_EQ(0, lucky7.o5Saved);
  EXPECT_EQ(0, lucky7.o6Saved);
  EXPECT_EQ(0, lucky7.o7Saved);

  EXPECT_EQ(0, lucky7.irTimeout);

  EXPECT_EQ(0, lucky7.aveptr);
              
  releaseArduinoMock();
}

TEST(Lucky7Test, SaveOutputState) {

  Lucky7 lucky7 = Lucky7();

  lucky7.o1Saved = 0;
  lucky7.o2Saved = 0;
  lucky7.o3Saved = 0;
  lucky7.o4Saved = 0;
  lucky7.o5Saved = 0;
  lucky7.o6Saved = 0;
  lucky7.o7Saved = 0;

  lucky7.o1 = 1;
  lucky7.o2 = 2;
  lucky7.o3 = 3;
  lucky7.o4 = 4;
  lucky7.o5 = 5;
  lucky7.o6 = 6;
  lucky7.o7 = 7;

  lucky7.saveOutputState();

  EXPECT_EQ(1, lucky7.o1Saved);
  EXPECT_EQ(2, lucky7.o2Saved);
  EXPECT_EQ(3, lucky7.o3Saved);
  EXPECT_EQ(4, lucky7.o4Saved);
  EXPECT_EQ(5, lucky7.o5Saved);
  EXPECT_EQ(6, lucky7.o6Saved);
  EXPECT_EQ(7, lucky7.o7Saved);
}

TEST(Lucky7Test, SetOutputStateFromSaved) {

  Lucky7 lucky7 = Lucky7();

  lucky7.o1 = 0;
  lucky7.o2 = 0;
  lucky7.o3 = 0;
  lucky7.o4 = 0;
  lucky7.o5 = 0;
  lucky7.o6 = 0;
  lucky7.o7 = 0;

  lucky7.o1Saved = 1;
  lucky7.o2Saved = 2;
  lucky7.o3Saved = 3;
  lucky7.o4Saved = 4;
  lucky7.o5Saved = 5;
  lucky7.o6Saved = 6;
  lucky7.o7Saved = 7;

  lucky7.setOutputStateFromSaved();

  EXPECT_EQ(1, lucky7.o1 );
  EXPECT_EQ(2, lucky7.o2 );
  EXPECT_EQ(3, lucky7.o3 );
  EXPECT_EQ(4, lucky7.o4 );
  EXPECT_EQ(5, lucky7.o5 );
  EXPECT_EQ(6, lucky7.o6 );
  EXPECT_EQ(7, lucky7.o7 );
}

