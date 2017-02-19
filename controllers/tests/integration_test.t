#include "integration_test.h"
#include "pins_arduino.h"
#include <IRremote.h>

using ::testing::_;
using ::testing::AnyOf;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::Return;

void Integration::SetUp() {
  pc1Value = 0;
  pc2Value = 0;
  bcValue  = 0;
}

void Integration::setAnalogReadValue(int pin, uint16_t pinValue) {
  switch (pin) {
  case A4:
    pc1Value = pinValue;
  case A5:
    pc2Value = pinValue;
  case A0:
    bcValue = pinValue;
  default:
    ADD_FAILURE() << "We should never get here.";
  }
}

void Integration::setAnalogReadPhotocellValues(uint16_t pinValues) {
  pc1Value = pinValues;
  pc2Value = pinValues;
}

void Integration::setAnalogReadVoltageValue(const float voltage) {
  bcValue = uint16_t(voltage/float(BVSCALE) + .5);
}



uint16_t Integration::getAnalogReadValue(int pin) {
  switch (pin) {
  case A4:
    return pc1Value;
  case A5:
    return pc2Value;
  case A0:
    return bcValue;
  default:
    ADD_FAILURE() << "We should never get here.";
    return 0;
  }
}

TEST_F(Integration, CycleThroughDay) {
  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(AtLeast(1));
  EXPECT_CALL(*irrecvMock, decode(_))
    .Times(AtLeast(1));

  EXPECT_CALL(*irrecvMock, resume())
    .Times(AtLeast(1));

  EEPROMMock * eepromMock  = EEPROMMockInstance();
  EXPECT_CALL(*eepromMock, read(_))
    .Times(AtLeast(1));

  SerialMock  * serialMock  = serialMockInstance();
  EXPECT_CALL(*serialMock, begin(_))
    .Times(1);
  EXPECT_CALL(*serialMock, available())
    .WillRepeatedly(Return(0));

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
  EXPECT_CALL(*arduinoMock, analogWrite(_,_))
    .Times(AtLeast(1));
  EXPECT_CALL(*arduinoMock, digitalWrite(_,_))
    .Times(AtLeast(1));
  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  Serial.setPrintToCout(true);

  setAnalogReadPhotocellValues(1000);
  setAnalogReadVoltageValue(12.0);
  
  arduinoMock->setMillisRaw(0);

  printContinuousStatus = true; // We want status sent to stdout
  
  setup();
  for (uint32_t i = 0; i < 2000; i++) {
    arduinoMock->addMillisRaw(1);
    loop();
  }
  EXPECT_EQ(MODE_BATTERYLOW, mode);
  for (uint32_t i = 0; i < 2; i++) {
    arduinoMock->addMillisRaw(1);
    loop();
  }

  EXPECT_EQ(MODE_DAY, mode);
  // Go for 1 hours
  for (uint32_t i = 0; i < 360000; i++) {
  // Go for 10 hours
  // for (uint32_t i = 0; i < 3600000; i++) {
    arduinoMock->addMillisRaw(10);
    loop();
  }
  setAnalogReadPhotocellValues(0);  // Night time
  // Go for 8 hours
  for (uint32_t i = 0; i < 100; i++) {
    arduinoMock->addMillisRaw(10);
    loop();
  }
  myIRKey = uint32_t('0');
  arduinoMock->addMillisRaw(10);
  loop();
  myIRKey = 0;
  for (uint32_t i = 0; i < 2880000; i++) {
    arduinoMock->addMillisRaw(10);
    loop();
  }
  // Go for 8 hours
  for (uint32_t i = 0; i < 1440000; i++) {
    arduinoMock->addMillisRaw(10);
    loop();
  }
   
  releaseArduinoMock();
  releaseSerialMock();
  releaseIRrecvMock();
  releaseEEPROMMock();

}

// TEST_F(Integration, TestNightMode) {
// }

