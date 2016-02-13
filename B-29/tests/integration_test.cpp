#include "integration_test.h"
#include "pins_arduino.h"
#include "B-29.ino"
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
    return pc1Value;
  case A4:
    return pc2Value;
  case A5:
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

  const uint16_t volts12 = uint16_t(12.0/float(BVSCALE) + .5);
  const uint16_t volts14 = uint16_t(14.0/float(BVSCALE) + .5);
  
  for (uint8_t j = 0; j < AVECNT; j++) {
    hw.pc1[j] = 0;
    hw.pc2[j] = 0;
    hw.bc[j] = volts12;
  }
  
  arduinoMock->setMillisRaw(0);
  
  setup();
  for (uint32_t i = 0; i < 2000; i++) {
    arduinoMock->addMillisRaw(1);
    for (uint8_t j = 0; j < AVECNT; j++) {
      hw.pc1[j] = 0;
      hw.pc2[j] = 0;
      hw.bc[j] = volts14;
    }
    loop();
  }
  EXPECT_EQ(MODE_BATTERYLOW, mode);
  for (uint32_t i = 0; i < 2; i++) {
    arduinoMock->addMillisRaw(1);
    for (uint8_t j = 0; j < AVECNT; j++) {
      hw.pc1[j] = 0;
      hw.pc2[j] = 0;
      hw.bc[j] = volts14;
    }
    loop();
  }
  EXPECT_EQ(MODE_DAY, mode);
  for (uint32_t i = 0; i < 3600000; i++) {
    arduinoMock->addMillisRaw(10);
    for (uint8_t j = 0; j < AVECNT; j++) {
      hw.pc1[j] = 0;
      hw.pc2[j] = 0;
      hw.bc[j] = volts14;
    }
    loop();
  }
  for (uint32_t i = 0; i < 2888000; i++) {
    arduinoMock->addMillisRaw(10);
    for (uint8_t j = 0; j < AVECNT; j++) {
      hw.pc1[j] = 1000;
      hw.pc2[j] = 1000;
      hw.bc[j] = volts14;
    }
    loop();
  }
  for (uint32_t i = 0; i < 1444000; i++) {
    arduinoMock->addMillisRaw(10);
    for (uint8_t j = 0; j < AVECNT; j++) {
      hw.pc1[j] = 0;
      hw.pc2[j] = 0;
      hw.bc[j] = volts14;
    }
    loop();
  }
   
  releaseArduinoMock();
  releaseSerialMock();
  releaseIRrecvMock();
  releaseEEPROMMock();

}

