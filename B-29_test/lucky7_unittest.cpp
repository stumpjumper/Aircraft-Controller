#include "lucky7.h"
#include "pins_arduino.h"
#include <IRremote.h>
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

  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);
  
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

  for (uint8_t i = 0; i < AVECNT; i++) {
    EXPECT_EQ(0, lucky7.pc1[i]);
    EXPECT_EQ(0, lucky7.pc2[i]);
    EXPECT_EQ(0, lucky7.bc [i]);
  }
              
  releaseArduinoMock();
  releaseIRrecvMock();
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

  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, resume())
    .Times(loopTimes);

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
  releaseIRrecvMock();
}

TEST(Lucky7Test, IRLoop) {
  uint32_t rv = 0;

  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(4)
    .WillRepeatedly(testing::InvokeWithoutArgs(
                      arduinoMock, &ArduinoMock::getMillis));

  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, resume())
    .Times(2);

  EXPECT_CALL(*irrecvMock, decode(_))
    .Times(2);
  
  Lucky7 lucky7 = Lucky7();

  irrecvMock->setIRValue(10);

  arduinoMock->setMillisRaw(0);
  rv = lucky7.irLoop();
  EXPECT_EQ(0,rv); // Should not have gone into ir read loop

  arduinoMock->setMillisRaw(250);
  rv = lucky7.irLoop();
  EXPECT_EQ(10,rv); // Should have gone into ir read loop and read value set above

  irrecvMock->setIRValue(20);

  arduinoMock->setMillisRaw(501);
  rv = lucky7.irLoop();
  EXPECT_EQ(0,rv); // Should not have gone into ir read loop and gotten default value

  arduinoMock->addMillisRaw(751);
  rv = lucky7.irLoop();
  EXPECT_EQ(20,rv); // Should have gone into ir read loop and read value set above

  releaseArduinoMock();
  releaseIRrecvMock();
}

TEST(Lucky7Test, Photocell1and2andBatteryVoltage) {

  Lucky7 lucky7 = Lucky7();

  for (uint8_t i = 0; i < AVECNT; i++) {
    lucky7.pc1[i] = i+1;
    lucky7.pc2[i] = i+1;
    lucky7.bc[i] = i+1;
  }

  const uint16_t sumPc1 = lucky7.photocell1();
  const uint16_t sumPc2 = lucky7.photocell2();
  float sumBc  = lucky7.batteryVoltage();
  EXPECT_EQ(5, sumPc1);
  EXPECT_EQ(5, sumPc2);
  EXPECT_GT(1.0e-6, fabs(sumBc - 5.0*BVSCALE));
}

TEST(Lucky7Test, OutputMoveTo) {

  
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, analogWrite(AnyOf(O1,O2),_))
    .Times(44);
  EXPECT_CALL(*arduinoMock, analogWrite(O3,_))
    .Times(11);
  EXPECT_CALL(*arduinoMock, analogWrite(O4,_))
    .Times(2);
  EXPECT_CALL(*arduinoMock, analogWrite(O5,_))
    .Times(2);
  EXPECT_CALL(*arduinoMock, analogWrite(O6,_))
    .Times(2);
  EXPECT_CALL(*arduinoMock, analogWrite(O7,_))
    .Times(2);
  EXPECT_CALL(*arduinoMock, analogWrite(O8,_))
    .Times(2);
  EXPECT_CALL(*arduinoMock, analogWrite(O13,_))
    .Times(2);

  Lucky7 lucky7 = Lucky7();

  lucky7.o1 = 10;
  lucky7.outputMoveTo(O1,lucky7.o1,20,999);
  EXPECT_EQ(20, lucky7.o1);

  lucky7.o2 = 20;
  lucky7.outputMoveTo(O2,lucky7.o2,10,999);
  EXPECT_EQ(10, lucky7.o2);

  lucky7.o3 = 100;
  lucky7.outputMoveTo(O3,lucky7.o3,100,999);
  EXPECT_EQ(100, lucky7.o3);

  lucky7.o1 = 0;
  lucky7.o1MoveTo(10,999);
  EXPECT_EQ(10, lucky7.o1);

  lucky7.o2 = 10;
  lucky7.o2MoveTo(0,999);
  EXPECT_EQ(0, lucky7.o2);

  lucky7.o3 = 0;
  lucky7.o3MoveTo(10,999);
  EXPECT_EQ(10, lucky7.o3);

  lucky7.o4 = 0;
  lucky7.o4MoveTo(1,999);
  EXPECT_EQ(1, lucky7.o4);

  lucky7.o5 = 1;
  lucky7.o5MoveTo(2,999);
  EXPECT_EQ(2, lucky7.o5);

  lucky7.o6 = 2;
  lucky7.o6MoveTo(3,999);
  EXPECT_EQ(3, lucky7.o6);

  lucky7.o7 = 3;
  lucky7.o7MoveTo(4,999);
  EXPECT_EQ(4, lucky7.o7);

  lucky7.o8 = 4;
  lucky7.o8MoveTo(5,999);
  EXPECT_EQ(5, lucky7.o8);

  lucky7.o13 = 5;
  lucky7.o13MoveTo(6,999);
  EXPECT_EQ(6, lucky7.o13);
  
  releaseArduinoMock();
  
}
