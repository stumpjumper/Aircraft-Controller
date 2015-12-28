#include "lucky7.h"
#include "pins_arduino.h"
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::AnyOf;
using ::testing::AtLeast;
using ::testing::Invoke;

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

  EXPECT_EQ(1, lucky7.o1);
  EXPECT_EQ(2, lucky7.o2);
  EXPECT_EQ(3, lucky7.o3);
  EXPECT_EQ(4, lucky7.o4);
  EXPECT_EQ(5, lucky7.o5);
  EXPECT_EQ(6, lucky7.o6);
  EXPECT_EQ(7, lucky7.o7);
}

int counterValue(int pin) {
  static int a0Counter = 0;
  static int a4Counter = 0;
  static int a5Counter = 0;

  switch (pin) {
  case A0:
    return a0Counter++;
  case A4:
    return a4Counter++;
  case A5:
    return a5Counter++;
  default:
    ADD_FAILURE() << "We should never get here.";
    return 0;
  }
}

TEST(Lucky7Test, Loop) {
  uint32_t rv = 0;

  ArduinoMock * arduinoMock = arduinoMockInstance();

  const uint8_t loopTimes = 3*AVECNT;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(loopTimes)
    .WillRepeatedly(testing::InvokeWithoutArgs(
                      arduinoMock, &ArduinoMock::getMillis));

  EXPECT_CALL(*arduinoMock, analogWrite(AnyOf(O1,O2,O3,O5,O6,O7),_))
    .Times(loopTimes*6);
  EXPECT_CALL(*arduinoMock, digitalWrite(AnyOf(O4,O8,O13),_))
    .Times(loopTimes*3);
           
  EXPECT_CALL(*arduinoMock, analogRead(A0))
    .Times(loopTimes)
    .WillRepeatedly(Invoke(counterValue));

  EXPECT_CALL(*arduinoMock, analogRead(A4))
    .Times(loopTimes)
    .WillRepeatedly(Invoke(counterValue));

  EXPECT_CALL(*arduinoMock, analogRead(A5))
    .Times(loopTimes)
    .WillRepeatedly(Invoke(counterValue));

  Lucky7 lucky7 = Lucky7();

  for (uint8_t j = 0; j < loopTimes; j++) {
    uint8_t i = j % AVECNT;
    rv = lucky7.loop();
    
    EXPECT_LT(i, AVECNT);
    EXPECT_EQ(j, lucky7.pc1[i]);
    EXPECT_EQ(j, lucky7.pc2[i]);
    EXPECT_EQ(j, lucky7.bc[i]);

    EXPECT_EQ(0, rv);
  }

  releaseArduinoMock();
}
