#include "integration_test.h"
#include "pins_arduino.h"
#include "B-29.ino"
#include <IRremote.h>

using ::testing::_;
using ::testing::AnyOf;
using ::testing::AtLeast;
using ::testing::Invoke;

void Integration::SetUp() {
  pc1Value = 0;
  pc2Value = 0;
  bcValue  = 0;
}

void Integration::setAnalogReadValue(int pin, uint16_t pinValue) {
  switch (pin) {
  case A0:
    pc1Value = pinValue;
  case A4:
    pc2Value = pinValue;
  case A5:
    bcValue = pinValue;
  default:
    ADD_FAILURE() << "We should never get here.";
  }
}

uint16_t Integration::getAnalogReadValue(int pin) {

  switch (pin) {
  case A0:
    std::cerr << "Integration::getAnalogReadValue: case A0" << std::endl;
    return pc1Value;
  case A4:
    std::cerr << "Integration::getAnalogReadValue: case A4" << std::endl;
    return pc2Value;
  case A5:
    std::cerr << "Integration::getAnalogReadValue: case A5" << std::endl;
    return bcValue;
  default:
    ADD_FAILURE() << "We should never get here.";
    return 0;
  }
}

TEST_F(Integration, CycleThroughDay) {
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, pinMode(AnyOf(O1,O2,O3,O4,O5,O6,O7,O8,O13),OUTPUT))
    .Times(9);
  
  EXPECT_CALL(*arduinoMock, pinMode(AnyOf(A0,A4,A5),INPUT))
    .Times(AtLeast(1));
  
  EXPECT_CALL(*arduinoMock, analogRead(A0))
    .Times(AtLeast(1))
    .WillRepeatedly(Invoke(this,&Integration::getAnalogReadValue));
  
  EXPECT_CALL(*arduinoMock, analogRead(A4))
    .Times(AtLeast(1))
    .WillRepeatedly(Invoke(this,&Integration::getAnalogReadValue));
  
  EXPECT_CALL(*arduinoMock, analogRead(A5))
    .Times(AtLeast(1))
    .WillRepeatedly(Invoke(this,&Integration::getAnalogReadValue));
  
  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);
    
  Lucky7 lucky7 = Lucky7();
  lucky7.setup();

  analogRead(A4);
  analogRead(A5);
  analogRead(A0);

  releaseArduinoMock();
  releaseIRrecvMock();
}


