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
// b1~ Cat Walk: Bomb Bay and Wheel Wells (d 13) 
// b2  Interior White: Flight Deck and Tail Gunner (nd, ??)  
// b3  Interior Red: Flight Deck and Tail Gunner (nd, ??)  
// b4  Cockpit Floods: Interior Cockpit Floods (nd, 2) 
// b5~ Loader~: (d, 4) 
// b6~ Tail Flood: Flood lights on tail flash~ (d, 2) 
// b7  Future

// Light objects to control each channel
DecayLight catwalk;       // Bomb Bay and Wheel Well lights, mostly on the catwalk (13)
Light      interiorWhite; // Interior White: Flight Deck and Tail Gunner (nd, ?
Light      interiorRed;   // Interior Red: Flight Deck and Tail Gunner (nd, ??)
Light      cockpitFloods; // Two lights behind the pilots (nd, 2)
DecayLight loader;        // The four lighs on the loader (d, 4)
Light      tailFloods;    // Two flood lighs on horizontal that illuminate the tail flash (2)

// Decay settings for catwalk lights during day and night
const uint8_t  catwalkMaxLightLevelDay   = uint8_t(.8*ON); // 80% Max
//const uint8_t  catwalkMaxLightLevelNight = uint8_t(.6*ON); // 60% Max
const uint8_t  catwalkMaxLightLevelNight = uint8_t(.5*ON);
uint32_t catwalkDayOnLengths[1]          = {180000}; // On 3 minutes
//uint32_t catwalkDayDecayLengths[1]       = { 61000}; // Off for 1 minute, plus 1 sec to keep out of sync
uint32_t catwalkDayDecayLengths[1]       = { 91000}; // Off for 90 seconds, plus 1 sec to keep out of sync
uint8_t  catwalkDayMaxLightLevels[1]     = {catwalkMaxLightLevelDay};  
uint32_t * catwalkDayTauInMilliseconds   = NULL;     // On/Off, no decay

// Flashing settings for loader's lights
const uint8_t  loaderMaxLightLevelDay    = uint8_t(.8*ON); // 80% Max
const uint8_t  loaderMaxLightLevelNight  = uint8_t(.6*ON); // 60% Max
uint32_t loaderDayOnLengths[1]           = {180000}; // On 3 minutes
uint32_t loaderDayDecayLengths[1]        = { 61500}; // Off for 1 minute, 15 seconds to keep out of sync
uint8_t  loaderDayMaxLightLevels[1]      = {loaderMaxLightLevelDay};  
uint32_t * loaderDayTauInMilliseconds    = NULL;     // On/Off, no decay

// Settings for tail floods
const uint8_t  tailFloodsMaxLightLevel   = uint8_t(0.8*ON); // 80% Max

void serialPrintBanner() {
    Serial.println(F("NMNSH B-52 Lighting Controller B v1.0"));
}

float getBatteryLowValue() {
  // Provide a non-default value if needed.  Default is BATTERYLOWDEFAULT
  // When voltage drops at or below this value, mode will switch to MODE_BATTERYLOW
  return 10.5;
}

float getBatteryLowResetValue() {
  // Provide a non-default value if needed.  Default is BATTERYLOWRESETDEFAULT
  // Will switch out of MODE_BATTERYLOW only after voltage rises at or above this value
  return 13.0;
}

bool overrideBatteryLow() {
  //Not used yet.  May be used when we have motors.  See B-29 for sample usage.
  return false;
}

void allLightsOn() {
  catwalk       .on();
  interiorWhite .on();
  interiorRed   .on();
  cockpitFloods .on();
  loader        .on();
  tailFloods    .on();
}

void allLightsOff() {
  catwalk       .off();
  interiorWhite .off();
  interiorRed   .off();
  cockpitFloods .off();
  loader        .off();
  tailFloods    .off();
}

void updateAll() {
  catwalk       .update();
  interiorWhite .update();
  interiorRed   .update();
  cockpitFloods .update();
  loader        .update();
  tailFloods    .update();
}

void allOff() {
  allLightsOff();
}

// -------------------- Time of Day Settings ----------------
void setEvening() {
  catwalkDayMaxLightLevels[0] = catwalkMaxLightLevelNight;  
  //  catwalk       .flash(); 2/18/17
  catwalk       .off();
  interiorWhite .off();
  // interiorRed   .on();
  interiorRed   .off();
  cockpitFloods .off();
  loaderDayMaxLightLevels[0] = loaderMaxLightLevelNight;
  // loader        .flash(); 2/18/17
  loader        .off();
  //tailFloods    .on(); 2/18/17
  tailFloods    .off();
}

void setNight() {
  allOff();
}

void setPreDawn() {
  //  catwalk       .flash(); 2/18/17
  catwalk       .off();
  interiorWhite .off();
  interiorRed   .off();
  cockpitFloods .off();
  loader        .off();
  //  tailFloods    .on(); 2/18/17
  tailFloods    .off();
}

void setMorning() {
  catwalk       .off();
  interiorWhite .off();
  interiorRed   .off();
  cockpitFloods .off();
  loader        .off();
  tailFloods    .off();
}

void setDay() {
  catwalkDayMaxLightLevels[0] = catwalkMaxLightLevelDay;  
  catwalk       .flash();
  interiorWhite .off();
  interiorRed   .off();
  cockpitFloods .off();
  loaderDayMaxLightLevels[0] = loaderMaxLightLevelDay;
  loader        .flash();
  tailFloods    .off();
}

void processKey(const uint32_t key) {
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
    catwalk.toggle();
    break;
  case '2':
  case RC65X_KEY2:
  case RM_YD065_KEY2:
    Serial.print(F("Got remote \"2\"\n"));
    setToMode(MODE_OVERRIDE);
    interiorWhite.toggle();
    break;
  case '3':
  case RC65X_KEY3:
  case RM_YD065_KEY3:
    Serial.print(F("Got remote \"3\"\n"));
    setToMode(MODE_OVERRIDE);
    interiorRed.toggle();
    break;
  case '4':
  case RC65X_KEY4:
  case RM_YD065_KEY4:
    Serial.print(F("Got remote \"4\"\n"));
    setToMode(MODE_OVERRIDE);
    cockpitFloods.toggle();
    break;
  case '5':
  case RC65X_KEY5:
  case RM_YD065_KEY5:
    Serial.print(F("Got remote \"5\"\n"));
    setToMode(MODE_OVERRIDE);
    loader.toggle();
    break;
  case '6':
  case RC65X_KEY6:
  case RM_YD065_KEY6:
    Serial.print(F("Got remote \"6\"\n"));
    setToMode(MODE_OVERRIDE);
    tailFloods.toggle();
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
  //                             1        2              3      
  serialPrintCustomStatusDefault(&catwalk,&interiorWhite,&interiorRed,
                                 &cockpitFloods,&loader,&tailFloods,NULL);
  //                             4              5       6           7
}

void setupLightingAndMotorChannels()
{
  catwalk      .setup(hw.o1, catwalkMaxLightLevelDay, 
                      1, catwalkDayOnLengths, catwalkDayDecayLengths,
                      catwalkDayMaxLightLevels, catwalkDayTauInMilliseconds);
  interiorWhite.setup(hw.o2, ON);
  interiorRed  .setup(hw.o3, ON);
  cockpitFloods.setup(hw.o4, ON);
  loader       .setup(hw.o5, loaderMaxLightLevelDay, 
                      1, loaderDayOnLengths, loaderDayDecayLengths,
                      loaderDayMaxLightLevels, loaderDayTauInMilliseconds);
  tailFloods   .setup(hw.o6, tailFloodsMaxLightLevel);
}
