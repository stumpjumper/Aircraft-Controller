
#include "B29_unittest.h"
#include "B-29.ino"
#include <IRremote.h>
#include <gtest/gtest.h>

using testing::AtLeast;
using testing::_;

TEST_F(B29Test, ArduinoMockMillis) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  unsigned long currentValue = 0;
  unsigned long addValue = 0;
  

  EXPECT_EQ(0, millis());

  currentValue = 0;
  arduinoMock->setMillisRaw(currentValue);
  EXPECT_EQ(currentValue, millis());

  currentValue = 9;
  arduinoMock->setMillisRaw(currentValue);
  EXPECT_EQ(currentValue, millis());

  currentValue = 11;
  arduinoMock->setMillisSecs(currentValue);
  EXPECT_EQ(currentValue*1000, millis());

  currentValue = 21;
  arduinoMock->setMillisMins(currentValue);
  EXPECT_EQ(currentValue*60*1000, millis());

  currentValue = 31;
  arduinoMock->setMillisHrs(currentValue);
  EXPECT_EQ(currentValue*60*60*1000, millis());

  currentValue = 0;
  arduinoMock->setMillisRaw(0);

  addValue = 11;
  currentValue += addValue*1000;
  arduinoMock->addMillisSecs(addValue);
  EXPECT_EQ(currentValue, millis());

  addValue = 21;
  currentValue += addValue*60*1000;
  arduinoMock->addMillisMins(addValue);
  EXPECT_EQ(currentValue, millis());

  addValue = 31;
  currentValue += addValue*60*60*1000;
  arduinoMock->addMillisHrs(addValue);
  EXPECT_EQ(currentValue, millis());

  //----------- Test when hours is a float
  float addHrs = 4.5;
  currentValue += 4*60*60*1000 + 30*60*1000;
  arduinoMock->addMillisHrs(addHrs);
  EXPECT_EQ(currentValue, millis());

  addHrs = .25;
  currentValue += 60*60*250;
  arduinoMock->addMillisHrs(addHrs);
  EXPECT_EQ(currentValue, millis());

  addHrs = .1;
  currentValue += 60*60*100;
  arduinoMock->addMillisHrs(addHrs);
  EXPECT_EQ(currentValue, millis());

  releaseArduinoMock();
}

TEST_F(B29Test, ResetTimeoutBatteryLow) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  arduinoMock->setMillisRaw(0);
  resetTimeoutBatteryLow();
  EXPECT_EQ(0+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  arduinoMock->setMillisRaw(10);
  resetTimeoutBatteryLow();
  EXPECT_EQ(10+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  arduinoMock->setMillisRaw(20);
  resetTimeoutBatteryLow();
  EXPECT_EQ(20+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  arduinoMock->setMillisRaw(40000);
  resetTimeoutBatteryLow();
  EXPECT_EQ(40000+TIMEOUTBATTERYLOW, timeoutBatteryLow);

  releaseArduinoMock();
}

TEST_F(B29Test, HardWareSetup) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(_,_))
    .Times(testing::AtLeast(1));

  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);
  
  hw.setup();
  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o7 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);

  releaseArduinoMock();
  releaseIRrecvMock();
}


TEST_F(B29Test, OverrideBatteryLow) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(_,_))
    .Times(testing::AtLeast(1));

  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);

  hw.setup();

  hw.o3 = ON;
  hw.o7 = ON;
  EXPECT_EQ(true, overrideBatteryLow());

  hw.o3 = ON;
  hw.o7 = OFF;
  EXPECT_EQ(true, overrideBatteryLow());

  hw.o3 = OFF;
  hw.o7 = ON;
  EXPECT_EQ(true, overrideBatteryLow());

  hw.o3 = OFF;
  hw.o7 = OFF;
  EXPECT_EQ(false, overrideBatteryLow());
  
  releaseArduinoMock();
  releaseIRrecvMock();
}

TEST_F(B29Test, ResetTimeoutOverride) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  arduinoMock->setMillisRaw(0);
  resetTimeoutOverride();
  EXPECT_EQ(0+TIMEOUTOVERRIDE, timeoutOverride);

  arduinoMock->setMillisRaw(10);
  resetTimeoutOverride();
  EXPECT_EQ(10+TIMEOUTOVERRIDE, timeoutOverride);

  arduinoMock->setMillisRaw(20);
  resetTimeoutOverride();
  EXPECT_EQ(20+TIMEOUTOVERRIDE, timeoutOverride);

  arduinoMock->setMillisRaw(30);
  resetTimeoutOverride();
  EXPECT_EQ(30+TIMEOUTOVERRIDE, timeoutOverride);

  releaseArduinoMock();
}

TEST_F(B29Test, NameLightGroupsOnAndOff) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(_,_))
    .Times(testing::AtLeast(1));
  
  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);

  hw.setup();

  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o7 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);

  ident.on();
  EXPECT_EQ(ON, hw.o1 );
  landing.on();
  EXPECT_EQ(ON, hw.o2 );
  illum.on();
  EXPECT_EQ(ON, hw.o4 );
  position.on();
  EXPECT_EQ(ON, hw.o5 );
  formation.on();
  EXPECT_EQ(ON, hw.o6 );
  blueLight.on ();
  EXPECT_EQ(ON, hw.o8 );
  redLight.on();
  EXPECT_EQ(ON, hw.o13 );

  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  ident.off();
  EXPECT_EQ(OFF, hw.o1 );
  landing.off();
  EXPECT_EQ(OFF, hw.o2 );
  illum.off();
  EXPECT_EQ(OFF, hw.o4 );
  position.off();
  EXPECT_EQ(OFF, hw.o5 );
  formation.off();
  EXPECT_EQ(OFF, hw.o6 );
  blueLight.off();
  EXPECT_EQ(OFF, hw.o8 );
  redLight.off ();
  EXPECT_EQ(OFF, hw.o13 );

  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  releaseArduinoMock();
  releaseIRrecvMock();
}

TEST_F(B29Test, AllLightsOnAndOff) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, pinMode(_,_))
    .Times(testing::AtLeast(1));
  
  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);

  hw.setup();

  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o7 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);

  allLightsOn();
  EXPECT_EQ(ON, hw.o1 );
  EXPECT_EQ(ON, hw.o2 );
  EXPECT_EQ(ON, hw.o4 );
  EXPECT_EQ(ON, hw.o5 );
  EXPECT_EQ(ON, hw.o6 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  allLightsOff();
  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13);
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );

  releaseArduinoMock();
  releaseIRrecvMock();
}

TEST_F(B29Test, UpdateLights) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  upDownMotor.setup(hw.o3, hw.o7); // (up, down)

  // On construction, all lights and motors are off
  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(OFF, redLight());

  // At time = 200 msecs decay lights are not yet decaying as their on-time is 250 msecs
  // on/off lights are whatever they are set to
  // Fast blinking lights are still on as have 1000 msec on time
  // Motor has 2 second delay before it starts
  arduinoMock->setMillisRaw(200);
  landing.on();
  illum.on();
  upDownMotor.motorUpStart();
  redLight.setToFast();
  blueLight.setToFast();

  updateLights();
  std::cerr << "ident.getDecaying() = " << bool(ident.getDecaying()) << std::endl;
  EXPECT_EQ(decayMaxLightLevels[0], ident());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());
  EXPECT_EQ(decayMaxLightLevels[0], position());
  EXPECT_EQ(decayMaxLightLevels[0], formation());
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
  EXPECT_EQ(ON, blueLight());
  EXPECT_EQ(ON, redLight());

  // At time = 250+450 = 700 ms decay lights (with 250 ms on-time and tau = 450 ms)
  // are decaying
  // on/off lights don't change.
  // Fast blinking lights are still on as have 1000 msec on time
  // Motor still not started

  arduinoMock->setMillisRaw(250+450);
  updateLights();
  EXPECT_EQ(int(decayMaxLightLevels[0]*.368+.5), ident()); 
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());
  EXPECT_EQ(int(decayMaxLightLevels[0]*.368+.5), position());
  EXPECT_EQ(int(decayMaxLightLevels[0]*.368+.5), formation());
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
  EXPECT_EQ(ON, blueLight());
  EXPECT_EQ(ON, redLight());

  // At time = 1000 msecs decay lights are still decaying
  // on/off lights don't change.
  // Fast blinking lights have just turned off
  // Motor still not started

  arduinoMock->setMillisRaw(1000);
  updateLights();
  EXPECT_GT(decayMaxLightLevels[0], ident());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());
  EXPECT_GT(decayMaxLightLevels[0], position());
  EXPECT_GT(decayMaxLightLevels[0], formation());
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(OFF, redLight());

  // At time = 1100 msecs decay lights are back full on
  // on/off lights don't change.
  // Fast blinking lights are on
  // Motor still not started

  arduinoMock->setMillisRaw(1100);
  updateLights();
  EXPECT_EQ(decayMaxLightLevels[0], ident());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());
  EXPECT_EQ(decayMaxLightLevels[0], position());
  EXPECT_EQ(decayMaxLightLevels[0], formation());
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
  EXPECT_EQ(ON, blueLight());
  EXPECT_EQ(ON, redLight());

  // At time = 5000 ms motor has turned on
  arduinoMock->setMillisRaw(5000);
  updateLights();
  EXPECT_EQ(ON, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           

  // At time = 45200 ms, motor is just about turned off
  arduinoMock->setMillisRaw(45200);
  updateLights();
  EXPECT_EQ(ON, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           

  // At time = 45201 ms, motor has just turned off
  arduinoMock->setMillisRaw(45201);
  updateLights();
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
  
  releaseArduinoMock();
}

TEST_F(B29Test, Setup) {

  EEPROMMock * eepromMock  = EEPROMMockInstance();
  EXPECT_CALL(*eepromMock, read(_))
    .Times(2);

  SerialMock  * serialMock  = serialMockInstance();
  EXPECT_CALL(*serialMock, begin(_))
    .Times(1);
  EXPECT_CALL(*serialMock, println("NMNSH B-29 Lighting Controller setup"))
    .Times(1);
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, pinMode(_,_))
    .Times(11);

  EXPECT_CALL(*arduinoMock, millis())
    .Times(1);

  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);


  arduinoMock->setMillisRaw(0);
  setup();
 
  EXPECT_EQ(MODE_DAY, mode);

  releaseSerialMock();
  releaseArduinoMock();
  releaseIRrecvMock();
  releaseEEPROMMock();

}
