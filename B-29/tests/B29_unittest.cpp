#include "B29_unittest.h"
#include "B-29.ino"
#include <IRremote.h>


using ::testing::AtLeast;
using ::testing::_;
using ::testing::An;
using ::testing::TypedEq;
using ::testing::Return;

void B29Test::SetUp()
{
  // setupLightingAndMotorChannels();
}

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
  
  SerialMock  * serialMock  = serialMockInstance();
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("ERROR: Detected uint32_t overflow in resetTimeoutBatteryLow()\n")))
    .Times(1);
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));
  
  //                              ,   .  ,   .   ,   . ,  ,   .  ,  ,   .
  uint32_t times[] = {0,10U,100U,1000U,10000U,100000U,1000000U,10000000U,
                      // ,  ,   . ,  ,  ,   . ,  ,  ,   .
                      100000000U,1000000000U,2000000000U,MILLISIN30DAYS,0xFFFFFFFF};
  const size_t numTimes = sizeof(times)/sizeof(uint32_t);
  
  for (size_t i = 0; i < numTimes; i++) {
    
    const uint32_t time = times[i];
    arduinoMock->setMillisRaw(time);
    resetTimeoutBatteryLow();
    if (i < numTimes-1) {
      EXPECT_GT(uint32_t(timeoutBatteryLow), uint32_t(time)) << "i = " << i << ", timeoutBatteryLow = " <<  uint32_t(timeoutBatteryLow) << ", time = " << uint32_t(time);
    } else {
      EXPECT_LT(uint32_t(timeoutBatteryLow), uint32_t(time)) << "i = " << i << ", timeoutBatteryLow = " <<  uint32_t(timeoutBatteryLow) << ", time = " << uint32_t(time);
    }
    EXPECT_EQ(time+uint32_t(TIMEOUTBATTERYLOW), timeoutBatteryLow) << "i = " << i << ", time = " << uint32_t(time);
  }
  
  releaseArduinoMock();
  releaseSerialMock();
}

TEST_F(B29Test, ResetTimeoutOverride) {
  SerialMock  * serialMock  = serialMockInstance();
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("ERROR: Detected uint32_t overflow in resetTimeoutOverride()\n")))
    .Times(1);
  
  ArduinoMock * arduinoMock = arduinoMockInstance();
  
  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));
  
  //                              ,   .  ,   .   ,   . ,  ,   .  ,  ,   .
  uint32_t times[] = {0,10U,100U,1000U,10000U,100000U,1000000U,10000000U,
                      // ,  ,   . ,  ,  ,   . ,  ,  ,   .
                      100000000U,1000000000U,2000000000U,MILLISIN30DAYS,0xFFFFFFFF};
  const size_t numTimes = sizeof(times)/sizeof(uint32_t);
  
  for (size_t i = 0; i < numTimes; i++) {
    
    const uint32_t time = times[i];
    arduinoMock->setMillisRaw(time);
    resetTimeoutOverride();
    if (i < numTimes-1) {
      EXPECT_GT(uint32_t(timeoutOverride), uint32_t(time)) << "i = " << i << ", timeoutOverride = " <<  uint32_t(timeoutOverride) << ", time = " << uint32_t(time);
    } else {
      EXPECT_LT(uint32_t(timeoutOverride), uint32_t(time)) << "i = " << i << ", timeoutOverride = " <<  uint32_t(timeoutOverride) << ", time = " << uint32_t(time);
    }
    EXPECT_EQ(time+uint32_t(TIMEOUTOVERRIDE), timeoutOverride) << "i = " << i << ", time = " << uint32_t(time);
  }
  releaseArduinoMock();
  releaseSerialMock();
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

TEST_F(B29Test, NameLightGroupsOnAndOff) {

  setupLightingAndMotorChannels();

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
}

void testAllLightsOn() {

  hw.o1  = OFF;
  hw.o2  = OFF;
  hw.o4  = OFF;
  hw.o5  = OFF;
  hw.o6  = OFF;
  hw.o8  = OFF;
  hw.o13 = OFF;
  hw.o3  = OFF;
  hw.o7  = OFF;

  allLightsOn();

  EXPECT_EQ(ON , hw.o1 );
  EXPECT_EQ(ON , hw.o2 );
  EXPECT_EQ(ON , hw.o4 );
  EXPECT_EQ(ON , hw.o5 );
  EXPECT_EQ(ON , hw.o6 );
  EXPECT_EQ(OFF, hw.o8 );
  EXPECT_EQ(OFF, hw.o13 );
  EXPECT_EQ(OFF, hw.o3 );
  EXPECT_EQ(OFF, hw.o7 );
}

void testAllLightsOff() {
  hw.o1  = ON;
  hw.o2  = ON;
  hw.o4  = ON;
  hw.o5  = ON;
  hw.o6  = ON;
  hw.o8  = ON;
  hw.o13 = ON;
  hw.o3  = ON;
  hw.o7  = ON;

  allLightsOff();

  EXPECT_EQ(OFF, hw.o1 );
  EXPECT_EQ(OFF, hw.o2 );
  EXPECT_EQ(OFF, hw.o4 );
  EXPECT_EQ(OFF, hw.o5 );
  EXPECT_EQ(OFF, hw.o6 );
  EXPECT_EQ(ON, hw.o8 );
  EXPECT_EQ(ON, hw.o13);
  EXPECT_EQ(ON, hw.o3 );
  EXPECT_EQ(ON, hw.o7 );
}

TEST_F(B29Test, AllLightsOn) {

  setupLightingAndMotorChannels();

  testAllLightsOn();
}

TEST_F(B29Test, AllLightsOff) {

  setupLightingAndMotorChannels();

  testAllLightsOff();
}

TEST_F(B29Test, UpdateLights) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));


  // Override aircraft light setup so timeing is correct for tests below
  decayOnLengths[0]         = 250;
  decayDecayLengths[0]      = 1100-250;
  decayMaxLightLevels[0]    = ON;
  decayTauInMilliseconds[0] = 450;

  setupLightingAndMotorChannels();

  // Override red and blue light's setup so timeing is correct for tests below
  blueLight.slowLight.onLengthValues [0] = 2000;
  blueLight.slowLight.offLengthValues[0] =   10;
  blueLight.fastLight.onLengthValues [0] = 1000;
  blueLight.fastLight.offLengthValues[0] =   10;

  redLight .slowLight.onLengthValues [0] = 2000;
  redLight .slowLight.offLengthValues[0] =   10;
  redLight .fastLight.onLengthValues [0] = 1000;
  redLight .fastLight.offLengthValues[0] =   10;

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

  // At time = 200 msecs decay lights are not yet decaying as their
  //   on-time is 250 msecs
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
  timeoutUpdateLights = 0;
  updateLights();
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
  
  releaseArduinoMock();
}

TEST_F(B29Test, Setup) {

  // EEPROMMock * eepromMock  = EEPROMMockInstance();
  // EXPECT_CALL(*eepromMock, read(_))
  //   .Times(2);

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
 
  EXPECT_EQ(MODE_BATTERYLOW, mode);

  releaseSerialMock();
  releaseArduinoMock();
  releaseIRrecvMock();
  // releaseEEPROMMock();

}

void testAllOff() {
  allLightsOn();
  hw.o3 = ON; // Never want up and down motors both on in real life!!
  hw.o7 = ON;       

  EXPECT_EQ(ON, ident());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());
  EXPECT_EQ(ON, position());
  EXPECT_EQ(ON, formation());
  EXPECT_EQ(ON, hw.o3); 
  EXPECT_EQ(ON, hw.o7);           

  allOff();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, hw.o3); 
  EXPECT_EQ(OFF, hw.o7);           
}

TEST_F(B29Test, AllOff) {

  setupLightingAndMotorChannels();

  testAllOff();

}

void checkStatusLightsAllOff() {
  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());
  EXPECT_EQ(Light::LIGHT_OFF, redLight.getLightMode());
  EXPECT_EQ(Light::LIGHT_OFF, blueLight.getLightMode());
}


void checkOverrideStatusLights() {
  //EXPECT_EQ(ON, redLight());  // Flashing light could be OFF
  //EXPECT_EQ(ON, blueLight()); // Flashing light could be OFF
  EXPECT_EQ(false, redLight.getPaused());
  EXPECT_EQ(false, blueLight.getPaused());

  EXPECT_EQ(FastSlowBlinkingLight::SLOW, redLight.getSpeed());
  EXPECT_EQ(Light::LIGHT_FLASHING, redLight.getLightMode());
  EXPECT_EQ(FastSlowBlinkingLight::FAST, blueLight.getSpeed());
  EXPECT_EQ(Light::LIGHT_FLASHING, blueLight.getLightMode());
}

void checkBatteryLowStatusLights() {
  //EXPECT_EQ(ON, redLight());  // Flashing light could be OFF
  //EXPECT_EQ(ON, blueLight()); // Flashing light could be OFF
  EXPECT_EQ(false, redLight.getPaused());
  EXPECT_EQ(false, blueLight.getPaused());

  EXPECT_EQ(FastSlowBlinkingLight::FAST, redLight.getSpeed());
  EXPECT_EQ(Light::LIGHT_FLASHING, redLight.getLightMode());
  EXPECT_EQ(FastSlowBlinkingLight::SLOW, blueLight.getSpeed());
  EXPECT_EQ(Light::LIGHT_FLASHING, blueLight.getLightMode());
}

void checkEveningStatusLights() {
  //EXPECT_EQ(ON , blueLight());  // Flashing light could be OFF
  EXPECT_EQ(true , redLight.getPaused());
  EXPECT_EQ(false, blueLight.getPaused());

  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(Light::LIGHT_OFF, redLight.getLightMode());
  EXPECT_EQ(FastSlowBlinkingLight::SLOW, blueLight.getSpeed());
  EXPECT_EQ(Light::LIGHT_FLASHING, blueLight.getLightMode());
}

void checkNightStatusLights() {
  EXPECT_EQ(true , redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(Light::LIGHT_OFF, redLight.getLightMode());
  EXPECT_EQ(ON , blueLight());
  EXPECT_EQ(Light::LIGHT_ON, blueLight.getLightMode());
}

void checkPreDawnStatusLights() {
  //EXPECT_EQ(ON, redLight());  // Flashing light could be OFF
  EXPECT_EQ(true , blueLight.getPaused());
  EXPECT_EQ(false, redLight.getPaused());

  EXPECT_EQ(FastSlowBlinkingLight::SLOW, redLight.getSpeed());
  EXPECT_EQ(Light::LIGHT_FLASHING, redLight.getLightMode());
  EXPECT_EQ(ON, blueLight());
  EXPECT_EQ(Light::LIGHT_ON, blueLight.getLightMode());
}

void checkMorningStatusLights() {
  //EXPECT_EQ(ON , redLight());  // Flashing light could be OFF
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(false, redLight.getPaused());
  EXPECT_EQ(true , blueLight.getPaused());
  EXPECT_EQ(FastSlowBlinkingLight::SLOW, redLight.getSpeed());
  EXPECT_EQ(Light::LIGHT_FLASHING, redLight.getLightMode());
  EXPECT_EQ(Light::LIGHT_OFF, blueLight.getLightMode());
}

void checkDayStatusLights() {
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  EXPECT_EQ(ON , redLight());
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(Light::LIGHT_ON, redLight.getLightMode());
  EXPECT_EQ(Light::LIGHT_OFF, blueLight.getLightMode());
}

TEST_F(B29Test, SetOverride) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  setupLightingAndMotorChannels();
  timeoutUpdateLights = 0;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  allLightsOn();
  redLight.on();
  blueLight.on();

  EXPECT_EQ(ON, ident());
  EXPECT_EQ(ON, position());
  EXPECT_EQ(ON, formation());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());

  EXPECT_EQ(true, position.getPaused());

  EXPECT_EQ(ON, redLight());
  EXPECT_EQ(ON, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  setOverride();

  arduinoMock->setMillisRaw(11);
  updateLights();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());

  checkOverrideStatusLights();

  releaseArduinoMock();
}

TEST_F(B29Test, SetBatteryLow) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  setupLightingAndMotorChannels();
  timeoutUpdateLights = 0;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  allLightsOn();
  redLight.on();
  blueLight.on();

  EXPECT_EQ(ON, ident());
  EXPECT_EQ(ON, position());
  EXPECT_EQ(ON, formation());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());

  EXPECT_EQ(true, position.getPaused());

  EXPECT_EQ(ON, redLight());
  EXPECT_EQ(ON, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  setBatteryLow();

  arduinoMock->setMillisRaw(11);
  updateLights();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());

  checkBatteryLowStatusLights();

  releaseArduinoMock();
}

TEST_F(B29Test, SetEvening) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  setupLightingAndMotorChannels();
  timeoutUpdateLights = 0;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  allLightsOff();
  redLight.off();
  blueLight.off();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());

  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  setEvening();

  arduinoMock->setMillisRaw(11);
  updateLights();

  EXPECT_EQ(ON, ident());
  EXPECT_EQ(ON, position());
  EXPECT_EQ(ON, formation());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());

  EXPECT_EQ(false, position.getPaused());

  checkEveningStatusLights();

  releaseArduinoMock();
}

TEST_F(B29Test, SetNight) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  setupLightingAndMotorChannels();
  timeoutUpdateLights = 0;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  allLightsOff();
  redLight.off();
  blueLight.off();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());

  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  setNight();

  arduinoMock->setMillisRaw(11);
  updateLights();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());

  checkNightStatusLights();

  releaseArduinoMock();
}

TEST_F(B29Test, SetPreDawn) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  setupLightingAndMotorChannels();
  timeoutUpdateLights = 0;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  allLightsOff();
  redLight.off();
  blueLight.off();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());

  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  setPreDawn();

  arduinoMock->setMillisRaw(11);
  updateLights();

  EXPECT_EQ(ON, ident());
  EXPECT_EQ(ON, position());
  EXPECT_EQ(ON, formation());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());

  EXPECT_EQ(false, position.getPaused());

  checkPreDawnStatusLights();

  releaseArduinoMock();
}

TEST_F(B29Test, SetMorning) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  setupLightingAndMotorChannels();
  timeoutUpdateLights = 0;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  allLightsOff();
  redLight.off();
  blueLight.off();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());

  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  setMorning();

  arduinoMock->setMillisRaw(11);
  updateLights();

  EXPECT_EQ(ON, ident());
  EXPECT_EQ(ON, position());
  EXPECT_EQ(ON, formation());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(ON, illum());

  EXPECT_EQ(false, position.getPaused());

  checkMorningStatusLights();

  releaseArduinoMock();
}


TEST_F(B29Test, SetDay) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  setupLightingAndMotorChannels();
  timeoutUpdateLights = 0;

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  allLightsOff();
  redLight.off();
  blueLight.off();

  EXPECT_EQ(OFF, ident());
  EXPECT_EQ(OFF, position());
  EXPECT_EQ(OFF, formation());
  EXPECT_EQ(OFF, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(true, position.getPaused());
  EXPECT_EQ(OFF, redLight());
  EXPECT_EQ(OFF, blueLight());
  EXPECT_EQ(true, redLight.getPaused());
  EXPECT_EQ(true, blueLight.getPaused());

  setDay();

  arduinoMock->setMillisRaw(11);
  updateLights();

  EXPECT_EQ(ON, ident());
  EXPECT_EQ(ON, position());
  EXPECT_EQ(ON, formation());
  EXPECT_EQ(ON, landing());
  EXPECT_EQ(OFF, illum());

  EXPECT_EQ(false, position.getPaused());

  checkDayStatusLights();

  releaseArduinoMock();
}

TEST_F(B29Test, SetToMode) {

  ArduinoMock * arduinoMock = arduinoMockInstance();


  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  setupLightingAndMotorChannels();

  Mode modes[7] = {MODE_OVERRIDE, MODE_BATTERYLOW, MODE_EVENING,
                   MODE_NIGHT   , MODE_PREDAWN   , MODE_MORNING, MODE_DAY};

  typedef void (*check_status_function)(void);
  check_status_function checkStatus[] =
    {checkOverrideStatusLights, checkBatteryLowStatusLights,
     checkEveningStatusLights , checkNightStatusLights,
     checkPreDawnStatusLights , checkMorningStatusLights,
     checkDayStatusLights};

  uint32_t time = 10000;

  timeoutOverride   = 10000;
  timeoutBatteryLow = 20000;
  
  arduinoMock->setMillisRaw(time); 
  
  for (uint8_t i = 0; i < 7; i++) {
    
    redLight.off();
    blueLight.off();
    checkStatusLightsAllOff();
    
    mode = MODE_NOTSET;
    setToMode(modes[i]);
    EXPECT_EQ(modes[i], mode);
    if (mode == MODE_OVERRIDE) {
      EXPECT_EQ(TIMEOUTOVERRIDE + 10000, timeoutOverride);
    }
    if (mode == MODE_BATTERYLOW) {
      EXPECT_EQ(TIMEOUTBATTERYLOW + 30000, timeoutBatteryLow);
    }
    
    // Move lights to new state
    time += 10000;
    arduinoMock->setMillisRaw(time); 
    updateLights();
    
    checkStatus[i]();
    
    redLight.off();
    blueLight.off();
    setToMode(modes[i]);
    EXPECT_EQ(modes[i], mode);
    // Move lights to new state
    time += 10000;
    arduinoMock->setMillisRaw(time); 
    updateLights();
    
    checkStatusLightsAllOff();
    
    if (mode == MODE_OVERRIDE) {
      EXPECT_EQ(TIMEOUTOVERRIDE + 20000, timeoutOverride);
    }
    if (mode == MODE_BATTERYLOW) {
      EXPECT_EQ(TIMEOUTBATTERYLOW + 40000, timeoutBatteryLow);
    }
  }
  // Ressting of overrides should not have been called twice
  EXPECT_EQ(TIMEOUTOVERRIDE + 20000, timeoutOverride);
  EXPECT_EQ(TIMEOUTBATTERYLOW + 40000, timeoutBatteryLow);

  releaseArduinoMock();

}



TEST_F(B29Test, ProcessKey) {

  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  SerialMock  * serialMock  = serialMockInstance();
  EXPECT_CALL(*serialMock, begin(_))
    .Times(1);

  EXPECT_CALL(*serialMock, println(_,_))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("key ")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"0\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"1\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"2\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"4\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"5\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"6\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"8\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"B\"\n")))
    .Times(AtLeast(1));
  // EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"R\"\n")))
  //   .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"U\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"D\"\n")))
    .Times(AtLeast(1));
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("Got remote \"P\"\n")))
    .Times(AtLeast(1));

  // EEPROMMock * eepromMock  = EEPROMMockInstance();
  // EXPECT_CALL(*eepromMock, write(_,_))
  //   .Times(2);
  // EXPECT_CALL(*eepromMock, read(_))
  //   .WillOnce(Return(300))
  //   .WillOnce(Return(400));
  
  setupLightingAndMotorChannels();
  Serial.begin(115200);

  uint32_t time = 10000;

  timeoutOverride   = 10000;
  timeoutBatteryLow = 20000;
  
  arduinoMock->setMillisRaw(time); 

  //-------------------------------------------------------
  // '0' : allOff
  //-------------------------------------------------------
  redLight.off();
  blueLight.off();
  checkStatusLightsAllOff();
    
  mode = MODE_NOTSET;
  processKey('0');
  EXPECT_EQ(MODE_OVERRIDE, mode);
  testAllOff();
  EXPECT_EQ(TIMEOUTOVERRIDE + time, timeoutOverride);
    
  // Move lights to new state
  time += 10000;
  arduinoMock->setMillisRaw(time); 
  updateLights();
    
  checkOverrideStatusLights();
    
  redLight.off();
  blueLight.off();
  processKey('0');
  EXPECT_EQ(MODE_OVERRIDE, mode);
  testAllOff();
  EXPECT_EQ(TIMEOUTOVERRIDE + time, timeoutOverride);

  // Move lights to new state
  time += 10000;
  arduinoMock->setMillisRaw(time); 
  updateLights();
  
  checkStatusLightsAllOff();
    
  //-------------------------------------------------------
  // '8' : allLightsOn
  // 'P' : Play
  //-------------------------------------------------------

  typedef void (*check_status_function)(void);
  check_status_function checkStatus[] =
    {checkEveningStatusLights , checkNightStatusLights,
     checkPreDawnStatusLights , checkMorningStatusLights,
     checkDayStatusLights};

  size_t i;

  TimeOfDay::DayPart dayParts[] = {TimeOfDay::EVENING, TimeOfDay::NIGHT  ,
                                   TimeOfDay::PREDAWN, TimeOfDay::MORNING,
                                   TimeOfDay::DAY                         };
  size_t numDayParts = sizeof(dayParts)/sizeof(TimeOfDay::DayPart);
  assert (numDayParts == 5);

  timeOfDay.setUpdateAverageTestMode(true); // Allows us to test statemap easily

  for (i = 0; i < numDayParts; i++) {

    //-----------------------
    // '8' : allLightsOn
    //-----------------------
    timeOfDay.currentDayPart = dayParts[i];

    redLight.off();
    blueLight.off();
    checkStatusLightsAllOff();
    
    mode = MODE_NOTSET;
    processKey('8');
    EXPECT_EQ(MODE_OVERRIDE, mode);
    testAllLightsOn();
    EXPECT_EQ(TIMEOUTOVERRIDE + time, timeoutOverride);
    
    // Move lights to new state
    time += 10000;
    arduinoMock->setMillisRaw(time); 
    updateLights();
    
    checkOverrideStatusLights();
    
    redLight.off();
    blueLight.off();
    processKey('0');
    EXPECT_EQ(MODE_OVERRIDE, mode);
    testAllLightsOn();
    EXPECT_EQ(TIMEOUTOVERRIDE + time, timeoutOverride);
    
    // Move lights to new state
    time += 10000;
    arduinoMock->setMillisRaw(time); 
    updateLights();
    
    checkStatusLightsAllOff();

    //-----------------------
    // 'P' : Play
    //-----------------------
    redLight.off();
    blueLight.off();
    processKey('P');
    EXPECT_EQ(dayParts[i], mode);

    // Move lights to new state
    time += 10000;
    arduinoMock->setMillisRaw(time); 
    updateLights();
    checkStatus[i]();
  }

  //-------------------------------------------------------
  // '1','2','4','5','6' : toggle o1, toggle o2, toggle o3
  //                       toggle o4, toggle o5, toggle o6
  // Note: o3 and o7 are the motor up and down controlers
  //       that are tested with 'U' and 'D'
  //-------------------------------------------------------

  Light * p_lights[] = {&ident, &landing, &illum, &position, &formation};
  char    keys    [] = {   '1',  '2',  '4',  '5',  '6'};
  size_t  numKeys   = sizeof(keys)/sizeof(char);
  size_t  numLights = sizeof(p_lights)/sizeof(Light *);
  assert(numKeys == numLights);
  assert(numKeys == 5);

  for (uint8_t i = 0; i < numKeys; i++) {
    std::stringstream msgS;
    msgS << "Testing key '" << keys[i] << "'";
    std::string msg =  msgS.str();

    redLight.off();
    blueLight.off();
    checkStatusLightsAllOff();
    
    mode = MODE_NOTSET;
    processKey(keys[i]);
    EXPECT_EQ(MODE_OVERRIDE, mode)  << msg;
    EXPECT_EQ(TIMEOUTOVERRIDE + time, timeoutOverride) << msg;
    
    // Move lights to new state
    time += 10000;
    arduinoMock->setMillisRaw(time); 
    updateLights();
    EXPECT_EQ(ON,(*p_lights[i])()) << msg;
    
    checkOverrideStatusLights();
    
    redLight.off();
    blueLight.off();
    processKey(keys[i]);
    EXPECT_EQ(MODE_OVERRIDE, mode)  << msg;
    EXPECT_EQ(TIMEOUTOVERRIDE + time, timeoutOverride) << msg;

    // Move lights to new state
    time += 10000;
    arduinoMock->setMillisRaw(time); 
    updateLights();
    EXPECT_EQ(OFF,(*p_lights[i])())  << msg;
    
    checkStatusLightsAllOff();
  }
  
  //-------------------------------------------------------
  // 'B' : set to MODE_BATTERYLOW
  //-------------------------------------------------------

  redLight.off();
  blueLight.off();
  checkStatusLightsAllOff();
    
  mode = MODE_NOTSET;
  processKey('B');
  EXPECT_EQ(MODE_BATTERYLOW, mode);
  testAllOff();
  EXPECT_EQ(TIMEOUTBATTERYLOW + time, timeoutBatteryLow);
    
  // Move lights to new state
  time += 10000;
  arduinoMock->setMillisRaw(time);
  updateLights();
    

  checkBatteryLowStatusLights();
    
  redLight.off();
  blueLight.off();
  processKey('B');
  EXPECT_EQ(MODE_BATTERYLOW, mode);
  testAllOff();
  EXPECT_EQ(TIMEOUTBATTERYLOW + time, timeoutBatteryLow);

  // Move lights to new state
  time += 10000;
  arduinoMock->setMillisRaw(time); 
  updateLights();
  
  checkStatusLightsAllOff();
  
  //-------------------------------------------------------
  // 'R' : read photocell and reset photocell value
  //-------------------------------------------------------

  // EXPECT_EQ(0, lightThreshold);
  // processKey('R');
  // EXPECT_EQ(11408, lightThreshold);
  
  //-------------------------------------------------------
  // 'U' : motor Up
  //-------------------------------------------------------

  EXPECT_EQ(false, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(false, upDownMotor.getInMotorDownMode());
  processKey('U');
  updateLights();
  EXPECT_EQ(true, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(false, upDownMotor.getInMotorDownMode());
  arduinoMock->addMillisRaw(LUCKY7_TIMEOUTMOTORUPDOWN/2); 
  updateLights();
  EXPECT_EQ(true, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(false, upDownMotor.getInMotorDownMode());
  arduinoMock->addMillisRaw(LUCKY7_TIMEOUTMOTORUPDOWN+1); 
  updateLights();
  EXPECT_EQ(false, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(false, upDownMotor.getInMotorDownMode());
  
  //-------------------------------------------------------
  // 'D' : motor Down
  //-------------------------------------------------------

  EXPECT_EQ(false, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(false, upDownMotor.getInMotorDownMode());
  processKey('D');
  updateLights();
  EXPECT_EQ(false, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(true, upDownMotor.getInMotorDownMode());
  arduinoMock->addMillisRaw(LUCKY7_TIMEOUTMOTORUPDOWN/2); 
  updateLights();
  EXPECT_EQ(false, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(true, upDownMotor.getInMotorDownMode());
  arduinoMock->addMillisRaw(LUCKY7_TIMEOUTMOTORUPDOWN+1); 
  updateLights();
  EXPECT_EQ(false, upDownMotor.getInMotorUpMode());
  EXPECT_EQ(false, upDownMotor.getInMotorDownMode());

  releaseSerialMock();
  releaseArduinoMock();
  // releaseEEPROMMock();

}

TEST_F(B29Test, Statemap) {

  size_t i;

  TimeOfDay::DayPart dayParts[] = {TimeOfDay::EVENING, TimeOfDay::NIGHT  ,
                                   TimeOfDay::PREDAWN, TimeOfDay::MORNING,
                                   TimeOfDay::DAY                         };
  size_t numDayParts = sizeof(dayParts)/sizeof(TimeOfDay::DayPart);
  

  ArduinoMock * arduinoMock = arduinoMockInstance();
  EXPECT_CALL(*arduinoMock, pinMode(_,_))
    .Times(11);

  EXPECT_CALL(*arduinoMock, millis())
    .Times(AtLeast(1));

  // EEPROMMock * eepromMock  = EEPROMMockInstance();
  // EXPECT_CALL(*eepromMock, read(_))
  //   .Times(2);

  SerialMock  * serialMock  = serialMockInstance();
  EXPECT_CALL(*serialMock, begin(_))
    .Times(1);
  EXPECT_CALL(*serialMock, println("NMNSH B-29 Lighting Controller setup"))
    .Times(1);
  EXPECT_CALL(*serialMock, print(TypedEq<const char *>("ERROR: Detected uint32_t overflow in resetTimeoutBatteryLow()\n")))
    .Times(0);

  IRrecvMock * irrecvMock = irrecvMockInstance();
  EXPECT_CALL(*irrecvMock, enableIRIn())
    .Times(1);

  arduinoMock->setMillisRaw(0);
  setup();
  timeOfDay.setUpdateAverageTestMode(true); // Allows us to test statemap easily

  EXPECT_EQ(MODE_BATTERYLOW, mode);
  EXPECT_EQ(TimeOfDay::DAY, timeOfDay.getDayPart());

  //----------------------------------------------------------------------------
  // Normal path where state set on basis of timeOfDay.updateAverage(lightLevel)
  //----------------------------------------------------------------------------

  for (i = 0; i < AVECNT; i++) {
    hw.pc1[i] = 1000;
    hw.pc2[i] = 1000;
    hw.bc[i] = uint16_t(12.0/float(BVSCALE) + .5);
  }

  EXPECT_EQ(TimeOfDay::DAY, timeOfDay.getDayPart());

  arduinoMock->addMillisRaw(3000);
  timeOfDay.update5minTimeout = arduinoMock->getMillis() + LUCKY7_TIME5MIN;

  for (i = 0; i < numDayParts; i++) {
    timeOfDay.currentDayPart = dayParts[i];
    statemap();
    EXPECT_EQ(dayParts[i], timeOfDay.getDayPart()) << "Current DayPart = "
                                                   << dayParts[i];
    EXPECT_EQ(dayParts[i], mode) << "Current DayPart = "
                                 << dayParts[i];
  }

  //----------------------------------------------------------------------------
  // case MODE_OVERRIDE
  //----------------------------------------------------------------------------

  // 1: millis() < timeoutOverride
  // 2: millis() > timeoutOverride


  timeOfDay.currentDayPart = TimeOfDay::PREDAWN;
  statemap();
  EXPECT_EQ(TimeOfDay::PREDAWN, timeOfDay.getDayPart());
  EXPECT_EQ(TimeOfDay::PREDAWN, mode);


  for (i = 0; i < numDayParts; i++) {

    timeOfDay.currentDayPart = dayParts[i];
    arduinoMock->addMillisRaw(1000);
    setToMode(MODE_OVERRIDE);
    EXPECT_EQ(MODE_OVERRIDE, mode);

    // millis() < timeoutOverride
    arduinoMock->addMillisRaw(10);
    statemap();
    EXPECT_EQ(MODE_OVERRIDE, mode);
    
    // millis() > timeoutOverride
    arduinoMock->addMillisRaw(TIMEOUTOVERRIDE);
    statemap();

    EXPECT_EQ(dayParts[i], timeOfDay.getDayPart()) << "Current DayPart = "
                                                   << dayParts[i];
    EXPECT_EQ(dayParts[i], mode) << "Current DayPart = "
                                 << dayParts[i];
  }

  //----------------------------------------------------------------------------
  // case MODE_BATTERYLOW
  //----------------------------------------------------------------------------

  // 1: millis() < timeoutBatteryLow, batteryVoltage <= BATTERYLOW
  // 2: millis() > timeoutBatteryLow, batteryVoltage <= BATTERYLOW
  // 3: millis() < timeoutBatteryLow, batteryVoltage > BATTERYLOW
  // 4: millis() > timeoutBatteryLow, batteryVoltage > BATTERYLOW

  timeOfDay.currentDayPart = TimeOfDay::PREDAWN;
  statemap();
  EXPECT_EQ(TimeOfDay::PREDAWN, timeOfDay.getDayPart());
  EXPECT_EQ(TimeOfDay::PREDAWN, mode);


  for (i = 0; i < AVECNT; i++) {
    hw.bc[i] = uint16_t((BATTERYLOW/2.0)/float(BVSCALE) + .5);
  }

  for (i = 0; i < numDayParts; i++) {

    timeOfDay.currentDayPart = dayParts[i];
    arduinoMock->addMillisRaw(1000);
    setToMode(MODE_BATTERYLOW);
    EXPECT_EQ(MODE_BATTERYLOW, mode);

    // millis() < timeoutBatteryLow, batteryVoltage <= BATTERYLOW
    arduinoMock->addMillisRaw(10);
    statemap();
    EXPECT_EQ(MODE_BATTERYLOW, mode);
    
    // millis() > timeoutBatteryLow, batteryVoltage <= BATTERYLOW
    arduinoMock->addMillisRaw(TIMEOUTBATTERYLOW);
    statemap();
    EXPECT_EQ(timeoutBatteryLow, arduinoMock->getMillis() + TIMEOUTBATTERYLOW);
    
    EXPECT_EQ(dayParts[i], timeOfDay.getDayPart()) << "Current DayPart = "
                                                   << dayParts[i];
    EXPECT_EQ(MODE_BATTERYLOW, mode) << "Current DayPart = "
                                     << dayParts[i];
  }

  timeOfDay.currentDayPart = TimeOfDay::PREDAWN;
  statemap();
  EXPECT_EQ(TimeOfDay::PREDAWN, timeOfDay.getDayPart());
  EXPECT_EQ(MODE_BATTERYLOW, mode);


  for (i = 0; i < AVECNT; i++) {
    hw.bc[i] = uint16_t(12.0/float(BVSCALE) + .5);
  }

  for (i = 0; i < numDayParts; i++) {

    timeOfDay.currentDayPart = dayParts[i];
    arduinoMock->addMillisRaw(1000);
    setToMode(MODE_BATTERYLOW);
    EXPECT_EQ(MODE_BATTERYLOW, mode);
    EXPECT_EQ(timeoutBatteryLow, arduinoMock->getMillis() + TIMEOUTBATTERYLOW);
    uint32_t timeoutBatteryLowOld = timeoutBatteryLow;

    // millis() < timeoutBatteryLow, batteryVoltage > BATTERYLOW
    arduinoMock->addMillisRaw(10);
    statemap();
    EXPECT_EQ(MODE_BATTERYLOW, mode);
    
    // millis() > timeoutBatteryLow, batteryVoltage > BATTERYLOW
    arduinoMock->addMillisRaw(TIMEOUTBATTERYLOW);
    statemap();
    EXPECT_EQ(timeoutBatteryLowOld, timeoutBatteryLow);
    
    EXPECT_EQ(dayParts[i], timeOfDay.getDayPart()) << "Current DayPart = "
                                                   << dayParts[i];
    EXPECT_EQ(dayParts[i], mode) << "Current DayPart = "
                                 << dayParts[i];
  }

  //----------------------------------------------------------------------------
  // Entering function with mode != MODE_BATTERYLOW
  //----------------------------------------------------------------------------

  uint8_t j;

  // 1: overrideBatteryLow() == true , batteryVoltage >= BATTERYLOW
  // Set values so overrideBatteryLow() will return true
  hw.o3 = ON;
  // Set battery so batteryVoltage >= BATTERYLOW
  for (i = 0; i < AVECNT; i++) {
    hw.bc[i] = uint16_t((12.0)/float(BVSCALE) + .5);
  }
  for (i = 0; i < numDayParts; i++) {
    mode = dayParts[i];
    for (j = 0; j < numDayParts; j++) {
      timeOfDay.currentDayPart = dayParts[i];
      statemap();
      EXPECT_EQ(dayParts[i], timeOfDay.getDayPart());
      EXPECT_EQ(dayParts[i], mode);
    }
  }
  
  // 2: overrideBatteryLow() == true , batteryVoltage <= BATTERYLOW
  // Set values so overrideBatteryLow() will return true
  hw.o3 = ON;
  // Set battery so batteryVoltage <= BATTERYLOW
  for (i = 0; i < AVECNT; i++) {
    hw.bc[i] = uint16_t((BATTERYLOW/2)/float(BVSCALE) + .5);
  }
  for (i = 0; i < numDayParts; i++) {
    mode = dayParts[i];
    for (j = 0; j < numDayParts; j++) {
      timeOfDay.currentDayPart = dayParts[i];
      statemap();
      EXPECT_EQ(dayParts[i], timeOfDay.getDayPart());
      EXPECT_EQ(dayParts[i], mode);
    }
  }

  // 3: overrideBatteryLow() == false, batteryVoltage >= BATTERYLOW
  // Set values so overrideBatteryLow() will return false
  hw.o3 = OFF;
  hw.o7 = OFF;
  // Set battery so batteryVoltage >= BATTERYLOW
  for (i = 0; i < AVECNT; i++) {
    hw.bc[i] = uint16_t((12.0)/float(BVSCALE) + .5);
  }
  for (i = 0; i < numDayParts; i++) {
    mode = dayParts[i];
    for (j = 0; j < numDayParts; j++) {
      timeOfDay.currentDayPart = dayParts[i];
      statemap();
      EXPECT_EQ(dayParts[i], timeOfDay.getDayPart());
      EXPECT_EQ(dayParts[i], mode);
    }
  }

  // 4: overrideBatteryLow() == false, batteryVoltage <= BATTERYLOW
  hw.o3 = OFF;
  hw.o7 = OFF;
  // Set battery so batteryVoltage <= BATTERYLOW
  for (i = 0; i < AVECNT; i++) {
    hw.bc[i] = uint16_t((BATTERYLOW/2)/float(BVSCALE) + .5);
  }
  for (i = 0; i < numDayParts; i++) {
    mode = dayParts[i];
    for (j = 0; j < numDayParts; j++) {
      timeOfDay.currentDayPart = dayParts[i];
      statemap();
      EXPECT_EQ(dayParts[i], timeOfDay.getDayPart());
      EXPECT_EQ(MODE_BATTERYLOW, mode);
    }
  }

  releaseArduinoMock();
  releaseSerialMock();
  releaseIRrecvMock();
  // releaseEEPROMMock();
}
