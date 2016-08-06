#include "aircraft.h"
#include <EEPROM.h>

// There's an interrupt collision with the IR routines and the PWM
// Outputs 1, 5, 6 can be dimmed
// Outputs 2, 3, 7 cannont be dimmed
// Outputs 4, 8 and 13 are digital, cannot be dimmed
//
// Output 8 is the red led on the board
// Output 13 is the blue led on the board

// Output mapping to lights/motors
// Lights:
// 
// Output types: (d)  = dimable, (nd) = not dimable
//
// a1~   Taxi: Wings and Landing Gear (d,3)
// a2    Landing: Gear Doors, Rt Gear (nd,3)
// a3    Terrain: (nd,1)
// a4    Terrain Motor: (nd, motor) Not yet used
// a5~   Navigation: Wing Tip, Vertical Stab, Bomb Bay (d,5)
// a6~   (Anti-)Collision: (d,6)
// a7    (Anti-)Collision Motor: (nd, motor) Not yet used

// Light objects to control each channel
DecayLight      taxi;            // Taxi lights on right-fwd landing gear and lft & rht wing landing gear (3)
Light           landing;         // Landing lights on rht-fwd landing gear lft & rht fwd gear doors and (3)
Light           terrain;         // Terrain light just ahead of most fwd landing gear (1)
// UpDownMotor  terrainMotor;    // Terrain light up-down motor just ahead of most fwd landing gear (1)
DecayLight      navigation;      // Navigation lights on lft & right wing tips, two on vertical stab and on on rht bomb-bay door (5)
RotatingLight   collision;       // Anti-Collision lights on lft & rht top before vertical stab and bottom under 47 section (6)
// UpdDownMotor collisionMotor;  // Anti-Collision motor: On/Off motor that rotates Anti-Collision lights (3)


// Flashing settings for taxi lights during day and night
const uint8_t  taxiMaxLightLevelDay   = uint8_t(.8*ON); // 80% Max
const uint8_t  taxiMaxLightLevelNight = uint8_t(.6*ON); // 60% Max
uint32_t taxiOnLengths[1]             = {180000}; // On 3 minutes
uint32_t taxiDecayLengths[1]          = { 60000}; // Off for 1 minute
uint8_t  taxiMaxLightLevels[1]        = {taxiMaxLightLevelDay};  
uint32_t * taxiTauInMilliseconds      = NULL;     // On/Off, no decay

// Flashing settings for navigation lights
uint32_t navigationOnLengths[1]          = {100};  // On for .1 seconds
uint32_t navigationDecayLengths[1]       = {1100}; // Decay for 1.1
uint8_t  navigationMaxLightLevels[1]     = {ON};  
uint32_t navigationTauInMilliseconds[1]  = {100};  // Half-life = .1 seconds

// Flashing settings for collision lights
const uint8_t  collisionOnLightLevel   = ON;  // When light is not "rotating", just on, this is its intensity
const uint32_t collisionFlatLength     = 0;   // Time between 1/2 sine waves.  250 = 1/4 sec
const uint8_t  collisionFlatLightLevel = 0;   // Light level when between 1/2 sine waves
const uint32_t collisionPulseLength    = 736; // 1/2 period of sine wave. 1000 = 1 sec
// Value based on that found here: https://www.youtube.com/watch?v=PSKxUBRD40E. (35 flashes in 25.2 sec =.72, 35 in 26.21=.748, 36 in 26.67=.741)
const uint8_t  collisionMinLightLevel  = 20;  // Light level when sine wave starts
const uint8_t  collisionMaxLightLevel  = ON;  // Light level at peak of sine wave

void serialPrintBanner() {
    Serial.println(F("NMNSH B-52 Lighting Controller A v1.0"));
}

bool overrideBatteryLow() {
  //Not used yet.  May be used when we have motors.  See B-29 for sample usage.
  return false;
}

void allLightsOn() {
  taxi        .on();
  landing     .on();
  terrain     .on();
  navigation  .on();
  collision   .on();
}

void allLightsOff() {
  taxi        .off();
  landing     .off();
  terrain     .off();
  navigation  .off();
  collision   .off();
}

void updateAll() {
  taxi        .update();
  landing     .update();
  terrain     .update();
  navigation  .update();
  collision   .update();
}

void allOff() {
  allLightsOff();
}

// -------------------- Time of Day Settings ----------------
void setEvening() {
  taxiMaxLightLevels[0] = taxiMaxLightLevelNight;  
  taxi        .flash();
  landing     .on();
  terrain     .off();
  navigation  .flash();
  collision   .flash();
}

void setNight() {
  allOff();
}

void setPreDawn() {
  taxiMaxLightLevels[0] = taxiMaxLightLevelNight;  
  taxi        .flash();
  landing     .on();
  terrain     .off();
  navigation  .flash();
  collision   .flash();
}

void setMorning() {
  taxiMaxLightLevels[0] = taxiMaxLightLevelDay;  
  taxi        .flash();
  landing     .on();
  terrain     .off();
  navigation  .flash();
  collision   .flash();
}

void setDay() {
  taxiMaxLightLevels[0] = taxiMaxLightLevelDay;  
  taxi        .flash();
  landing     .on();
  terrain     .off();
  navigation  .flash();
  collision   .flash();
}

void processKey(uint32_t key) {
  Serial.print(F("key "));
  Serial.println(key, HEX);
  switch (key) {
  case '0':
  case RC65X_KEY0:
  case RC65X_KEYDOWN: // Control wheel down
  case RM_YD065_KEY0:
  case RM_YD065_KEYDOWN:
    Serial.print(F("Got remote \"0\"\n"));
    setToMode(MODE_OVERRIDE);
    allOff();
    break;
  case '1':
  case RC65X_KEY1:
  case RM_YD065_KEY1:
    Serial.print(F("Got remote \"1\"\n"));
    setToMode(MODE_OVERRIDE);
    taxi.toggle();
    break;
  case '2':
  case RC65X_KEY2:
  case RM_YD065_KEY2:
    Serial.print(F("Got remote \"2\"\n"));
    setToMode(MODE_OVERRIDE);
    landing.toggle();
    break;
  case '3':
  case RC65X_KEY3:
  case RM_YD065_KEY3:
    Serial.print(F("Got remote \"3\"\n"));
    setToMode(MODE_OVERRIDE);
    terrain.toggle();
    break;
  // case '4':
  // case RC65X_KEY4:
  // case RM_YD065_KEY4:
  //   Serial.print(F("Got remote \"4\"\n"));
  //   setToMode(MODE_OVERRIDE);
  //   something.toggle();
  //   break;
  case '5':
  case RC65X_KEY5:
  case RM_YD065_KEY5:
    Serial.print(F("Got remote \"5\"\n"));
    setToMode(MODE_OVERRIDE);
    navigation.toggle();
    break;
  case '6':
  case RC65X_KEY6:
  case RM_YD065_KEY6:
    Serial.print(F("Got remote \"6\"\n"));
    setToMode(MODE_OVERRIDE);
    collision.toggle();
    break;
//   case '7':
//   case RC65X_KEY7:
//   case RM_YD065_KEY7:
//     Serial.print(F("Got remote \"7\"\n"));
//     setToMode(MODE_OVERRIDE);
//     floods.toggle();
//     break;
  case 'B':
  case RC65X_KEYRED:
  case RM_YD065_KEYRED:
    Serial.print(F("Got remote \"B\"\n"));
    setToMode(MODE_BATTERYLOW);
    break;
  case '8':
  case RC65X_KEY8:
  case RC65X_KEYUP: // Control wheel up
  case RM_YD065_KEY8:
  case RM_YD065_KEYUP:
    Serial.print(F("Got remote \"8\"\n"));
    setToMode(MODE_OVERRIDE);
    allLightsOn();
    break;
  case 'P':
  case RC65X_KEYPLAY:
  case RC65X_KEYSELECT:
  case RM_YD065_KEYPLAY:
  case RM_YD065_KEYOK:
    Serial.print(F("Got remote \"P\"\n"));
    const uint16_t lightLevel = hw.photocell2();
    const TimeOfDay::DayPart dayPart = timeOfDay.updateAverage(lightLevel);
    setToMode(dayPart);
    break;
  }
}

void serialPrintCustomStatus()
{
  sprintf(sprintfBuffer,
          "|1:%1i:%3d|2:%3d|3:%3d|5:%1i:%3d|6:%1i:%3d,r:%3d|b:%3d|",
          int(taxi.getLightMode()), hw.o1,
          hw.o2,
          hw.o3,
          int(navigation.getLightMode()), hw.o5,
          int(collision.getLightMode()), hw.o6,
          hw.o13,hw.o8);
  Serial.print(sprintfBuffer);
}

void setupLightingAndMotorChannels()
{
  taxi       .setup(hw.o1, ON, 1, taxiOnLengths, taxiDecayLengths,
                    taxiMaxLightLevels, taxiTauInMilliseconds);
  landing    .setup(hw.o2, ON);
  terrain    .setup(hw.o3, ON);
  navigation .setup(hw.o5, ON, 1, navigationOnLengths, navigationDecayLengths,
                    navigationMaxLightLevels, navigationTauInMilliseconds);
  collision  .setup(hw.o6, collisionOnLightLevel, collisionFlatLength,          
                    collisionFlatLightLevel, collisionPulseLength,
                    collisionMinLightLevel, collisionMaxLightLevel);
}
