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
// a1~ Formation                     Blink
// a2  Tail Flash                    Steady
// a3  Belly                         Steady
// a5~ Position: Wing Tip, Tail Top  Blink (40 cycles/min)
// a6~ Anti-Smash                    Pulse (80 rpm)

// From the F-105D manual:
//
// Formation: Consisting of luna white lights, one each in the rudder fairing
// and one each mounted on aft fuselage, on each side of the vertical stabilizer
//
// Position: Consist of red (left) and green (right) lights in the wing tips and
// two white lights in the vertical fin tip, and white lights on the top, bottom
// and sides of the forward fuselage. The top fuselage light is enclosed within
// a transparent section of the turtledeck (back of cockpit to the fin).
//
// Position Light Switch: Switch has three positions: FLASH, OFF, and
// STEADY. When in STEADY position, the wing and tail lights will be
// illuminated.  When the switch is moved to FLASH position, the red, green, and
// white tail lights flash alternately at the rate of 40 cycles per minute.

// I think
// front lights and top of tail are position lights,
// back lights (exluding top of tail) are formation lights

// Light objects to control each channel
FlashingLight   formation;  // Not sure if these are formation or position
Light           tailFlash;  // Think this is a non-blinking formation
Light           belly    ;  // Think this is a position light
DecayLight      position ;  // Left & right wing tips and tail top (40 cyc/sec)
RotatingLight   collision;  // Anti-Collision lights on top and bottom (80 rpm)

// Flashing settings for formation lights
uint32_t formationOnLengths[1]         = {300000}; // On 5 minutes    
uint32_t formationOffLengths[1]        = { 60000}; // Off for 1 minute
uint8_t  formationMaxLightLevels[1]    = {ON};  

// Decay settings for position lights
uint32_t positionOnLengths[1]         = {100};  // On for .1 seconds
uint32_t positionDecayLengths[1]      = {1100}; // Decay for 1.4
uint8_t  positionMaxLightLevels[1]    = {ON};  
uint32_t positionTauInMilliseconds[1] = {100};  // Half-life = .1 seconds

// Rotation settings for collision lights
const uint8_t  collisionOnLightLevel   = ON;  // When light is not "rotating", just on, this is its intensity
const uint32_t collisionFlatLength     = 375;   // Time between 1/2 sine waves.  1000 = 1 sec
const uint8_t  collisionFlatLightLevel = 0;   // Light level when between 1/2 sine waves
const uint32_t collisionPulseLength    = 375; // 1/2 period of sine wave. 1000 = 1 sec
// Value based on that found here: https://www.youtube.com/watch?v=PSKxUBRD40E. (35 flashes in 25.2 sec =.72, 35 in 26.21=.748, 36 in 26.67=.741)
const uint8_t  collisionMinLightLevel  = 20;  // Light level when sine wave starts
const uint8_t  collisionMaxLightLevel  = ON;  // Light level at peak of sine wave

void serialPrintBanner() {
    Serial.println(F("NMNSH F-105 Lighting Controller v1.0"));
}

float getBatteryLowValue() {
  // Provide a non-default value if needed.  Default is BATTERYLOWDEFAULT
  // When voltage drops at or below this value, mode will switch to MODE_BATTERYLOW
  return 10.0;
}

float getBatteryLowResetValue() {
  // Provide a non-default value if needed.  Default is BATTERYLOWRESETDEFAULT
  // Will switch out of MODE_BATTERYLOW only after voltage rises at or above this value
  return BATTERYLOWRESETDEFAULT;
}

bool overrideBatteryLow() {
  //Not used yet.  May be used when we have motors.  See B-29 for sample usage.
  return false;
}

void allLightsOn() {
  formation.on();
  tailFlash.on();
  belly    .on();
  position .on();
  collision.on();
  
}

void allLightsOff() {
  formation.off();
  tailFlash.off();
  belly    .off();
  position .off();
  collision.off();
}

void updateAll() {
  formation.update();
  tailFlash.update();
  belly    .update();
  position .update();
  collision.update();
}

void allOff() {
  allLightsOff();
}

// -------------------- Time of Day Settings ----------------
void setEvening() {
  formation.flash();
  tailFlash.on();
  belly    .on();
  position .flash();
  collision.flash();
}

void setNight() {
  allOff();
}

void setPreDawn() {
  formation.flash();
  tailFlash.on();
  belly    .on();
  position .flash();
  collision.flash();
}

void setMorning() {
  formation.flash();
  tailFlash.on();
  belly    .on();
  position .flash();
  collision.flash();
}

void setDay() {
  formation.flash();
  tailFlash.on();
  belly    .on();
  position .flash();
  collision.flash();
}

void processKey(const uint32_t key) {
  switch (key) {
  case '?': // Print a single line of status
    status();
    break;
  case 'c': // Put into continuous status print mode
    status();
    break;
  case 's': // If on, stop continuous status print mode
    status();
    break;
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
    formation.toggle();
    break;
  case '2':
  case RC65X_KEY2:
  case RM_YD065_KEY2:
    Serial.print(F("Got remote \"2\"\n"));
    setToMode(MODE_OVERRIDE);
    tailFlash.toggle();
    break;
  case '3':
  case RC65X_KEY3:
  case RM_YD065_KEY3:
    Serial.print(F("Got remote \"3\"\n"));
    setToMode(MODE_OVERRIDE);
    belly.toggle();
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
    position.toggle();
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
  //                              1          2          3     4     5
  serialPrintCustomStatusDefault(&formation,&tailFlash,&belly,NULL,&position,
                                 &collision,NULL);
  //                              6         7
}

void setupLightingAndMotorChannels()
{
  formation  .setup(hw.o1, ON, 1, formationOnLengths, formationOffLengths,
                    formationMaxLightLevels);
  tailFlash  .setup(hw.o2, ON);
  belly      .setup(hw.o3, ON);
  position   .setup(hw.o5, ON, 1, positionOnLengths, positionDecayLengths,
                    positionMaxLightLevels, positionTauInMilliseconds);
  collision  .setup(hw.o6, collisionOnLightLevel, collisionFlatLength,          
                    collisionFlatLightLevel, collisionPulseLength,
                    collisionMinLightLevel, collisionMaxLightLevel);
}
