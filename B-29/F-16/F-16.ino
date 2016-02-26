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
//  Output types: (d)  = dimable, (nd) = not dimable
//
//  01 Not Used (d)
//  02 Taxi (nd)
//     Landing lights on rear wheels (2)
//  03 Not Used (nd)
//  04 Not Used (nd)
//  05 Position (d)
//     Wing Tips (2)
//     Intake Sides (2)
//  06 Collision (d)
//     Top of Tail (2)
//  07 Floods (nd)
//     Tail Illumination (2)

// Decay settings for position lights
uint32_t positionOnLengths[1]          = {100};  // On for .1 seconds
uint32_t positionDecayLengths[1]       = {1100}; // Decay for 1.1
uint8_t  positionMaxLightLevels[1]     = {ON};  
uint32_t positionTauInMilliseconds[1]  = {100};  // Half-life = .1 seconds

// Decay settings for collision lights
uint32_t collisionOnLengths[2]         = {50,50};   // On : .05s, .05s
uint32_t collisionDecayLengths[2]      = {250,1500};// Off: .25s, then 1.75 s
uint8_t  collisionMaxLightLevels[2]    = {ON,ON};      // Full power
uint32_t * collisionTauInMilliseconds  = NULL;     // On/Off, no decay

// Decay settings for taxi lights during day and night
uint32_t taxiDayOnLengths[1]           = {300000}; // On 5 minutes
uint32_t taxiDayDecayLengths[1]        = {300000}; // Off for 5 minutes
uint8_t  taxiDayMaxLightLevels[1]      = {ON};  
uint32_t * taxiDayTauInMilliseconds    = NULL;     // On/Off, no decay

// Light objects to control each channel
DecayLight taxi     ; // Taxi          : Landing lighs on rear wheels (2)
DecayLight position ; // Position      : Wing tips (2), Intake sides (2)
DecayLight collision; // Collision     : Top of tail (2)
Light      floods   ; // Floods        : Tail Illumination (2)

void serialPrintBanner() {
    Serial.println(F("NMNSH F-16 Lighting Controller"));
}

bool overrideBatteryLow() {
  //Not used on F-16.  See B-29 for sample usage
  return false;
}

void allLightsOn() {
  taxi     .on();
  position .on();
  collision.on();
  floods   .on();
}

void allLightsOff() {
  taxi     .off();
  position .off();
  collision.off();
  floods   .off();
}

void updateAll() {
  taxi     .update();
  position .update();
  collision.update();
  floods   .update();
  
  blueLight.update();
  redLight .update();
}

void allOff() {
  allLightsOff();
}

// -------------------- Time of Day Settings ----------------
void setEvening() {
  redLight .off();
  blueLight.setToSlow();
  blueLight.flash();
  
  taxi     .on();
  position .flash();
  collision.flash();
  floods   .on();
}

void setNight() {
  redLight .off();
  blueLight.on();
  
  allOff();
}

void setPreDawn() {
  redLight .setToSlow();
  redLight .flash();
  blueLight.on();
  
  taxi     .on();
  position .flash();
  collision.flash();
  floods   .on();
}

void setMorning() {
  redLight .setToSlow();
  redLight .flash();
  blueLight.off();
  
  taxi     .on();
  position .flash();
  collision.flash();
  floods   .on();
}

void setDay() {
  redLight  .on();
  blueLight.off();
  
  taxi     .flash();
  position .flash();
  collision.flash();
  floods   .off();
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
  // case '1':
  // case RC65X_KEY1:
  // case RM_YD065_KEY1:
  //   Serial.print(F("Got remote \"1\"\n"));
  //   setToMode(MODE_OVERRIDE);
  //   something.toggle();
  //   break;
  case '2':
  case RC65X_KEY2:
  case RM_YD065_KEY2:
    Serial.print(F("Got remote \"2\"\n"));
    setToMode(MODE_OVERRIDE);
    taxi.toggle();
    break;
  // case '3':
  // case RC65X_KEY3:
  // case RM_YD065_KEY3:
  //   Serial.print(F("Got remote \"3\"\n"));
  //   setToMode(MODE_OVERRIDE);
  //   something.toggle();
  //   break;
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
  case '7':
  case RC65X_KEY7:
  case RM_YD065_KEY7:
    Serial.print(F("Got remote \"7\"\n"));
    setToMode(MODE_OVERRIDE);
    floods.toggle();
    break;
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
  char buffer[75];
  sprintf(buffer,
          "|1:%3d|2:%1i:%3d|3:%3d|4:%3d|5:%1i:%3d|6:%1i:%3d|7:%3d,r:%3d|b:%3d|",
          hw.o1,
          int(taxi.getLightMode()), hw.o2,
          hw.o3,
          hw.o4,
          int(position.getLightMode()), hw.o5,
          int(collision.getLightMode()), hw.o6,
          hw.o7,
          hw.o13,hw.o8);
  
  Serial.print(buffer);
}

void setupLightingAndMotorChannels()
{
  taxi     .setup(hw.o2, ON, 1, taxiDayOnLengths, taxiDayDecayLengths,
                  taxiDayMaxLightLevels, taxiDayTauInMilliseconds);
  position .setup(hw.o5, ON, 1, positionOnLengths, positionDecayLengths,
                  positionMaxLightLevels, positionTauInMilliseconds);
  collision.setup(hw.o6, ON, 2, collisionOnLengths, collisionDecayLengths,
                  collisionMaxLightLevels, collisionTauInMilliseconds);
  floods   .setup(hw.o7, ON);

  blueLight.setup(hw.o8 , ON, ON);
  redLight .setup(hw.o13, ON, ON);
}
