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
//  01, 05, 06 are powered (d)
//  02, 03, 04, 07 are not powered (nd)

// Light objects to control each channel
Light  light1; 
Light  light5; 
Light  light6; 

void serialPrintBanner() {
    Serial.println(F("Adjustable Lights"));
}

bool overrideBatteryLow() {
  // Not used for these lights.  See B-29 for sample usage
  return false;
}

void allLightsOn() {
  Serial.println(F("In allLightsOn()"));
  light1.on();
  light5.on();
  light6.on();
}

void allLightsOff() {
  Serial.println(F("In allLightsOff()"));
  light1.off();
  light5.off();
  light6.off();
}

void updateAll() {
  light1.update();
  light5.update();
  light6.update();
}

void allOff() {
  Serial.println(F("In allOff()"));
  allLightsOff();
}

// -------------------- Time of Day Settings ----------------
void setEvening() {
}

void setNight() {
}

void setPreDawn() {
}

void setMorning() {
}

void setDay() {
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
    light1.setOnLightLevel(ON*.1);
    light5.setOnLightLevel(ON*.1);
    light6.setOnLightLevel(ON*.1);
    allLightsOn();
    break;
  case '2':
  case RC65X_KEY2:
  case RM_YD065_KEY2:
    Serial.print(F("Got remote \"2\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.2);
    light5.setOnLightLevel(ON*.2);
    light6.setOnLightLevel(ON*.2);
    allLightsOn();
    break;
  case '3':
  case RC65X_KEY3:
  case RM_YD065_KEY3:
    Serial.print(F("Got remote \"3\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.3);
    light5.setOnLightLevel(ON*.3);
    light6.setOnLightLevel(ON*.3);
    allLightsOn();
    break;
  case '4':
  case RC65X_KEY4:
  case RM_YD065_KEY4:
    Serial.print(F("Got remote \"4\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.4);
    light5.setOnLightLevel(ON*.4);
    light6.setOnLightLevel(ON*.4);
    allLightsOn();
    break;
  case '5':
  case RC65X_KEY5:
  case RM_YD065_KEY5:
    Serial.print(F("Got remote \"5\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.5);
    light5.setOnLightLevel(ON*.5);
    light6.setOnLightLevel(ON*.5);
    allLightsOn();
    break;
  case '6':
  case RC65X_KEY6:
  case RM_YD065_KEY6:
    Serial.print(F("Got remote \"6\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.6);
    light5.setOnLightLevel(ON*.6);
    light6.setOnLightLevel(ON*.6);
    allLightsOn();
    break;
  case '7':
  case RC65X_KEY7:
  case RM_YD065_KEY7:
    Serial.print(F("Got remote \"7\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.7);
    light5.setOnLightLevel(ON*.7);
    light6.setOnLightLevel(ON*.7);
    allLightsOn();
    break;
  case 'B':
  case RC65X_KEYRED:
  case RM_YD065_KEYRED:
    Serial.print(F("Got remote \"B\"\n"));
    setToMode(MODE_BATTERYLOW);
    allLightsOn();
    break;
  case '8':
  case RC65X_KEY8:
  case RM_YD065_KEY8:
    Serial.print(F("Got remote \"8\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.8);
    light5.setOnLightLevel(ON*.8);
    light6.setOnLightLevel(ON*.8);
    allLightsOn();
    break;
  case '9':
  case RC65X_KEY9:
  case RM_YD065_KEY9:
    Serial.print(F("Got remote \"9\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.setOnLightLevel(ON*.9);
    light5.setOnLightLevel(ON*.9);
    light6.setOnLightLevel(ON*.9);
    allLightsOn();
    break;
  case 'U':
  case RC65X_KEYCHANUP:
  case RM_YD065_KEYVOLUMEUP:
  case RM_YD065_KEYPROGUP:
    Serial.print(F("Got remote \"U\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.incrementOnLightLevel(5);
    light5.incrementOnLightLevel(5);
    light6.incrementOnLightLevel(5);
    allLightsOn();
    break;
  case 'D':
  case RC65X_KEYCHANDOWN:
  case RM_YD065_KEYVOLUMEDOWN:
  case RM_YD065_KEYPROGDOWN:
    Serial.print(F("Got remote \"D\"\n"));
    setToMode(MODE_OVERRIDE);
    light1.incrementOnLightLevel(-5);
    light5.incrementOnLightLevel(-5);
    light6.incrementOnLightLevel(-5);
    allLightsOn();
    break;
  case 'P':
  case RC65X_KEYPLAY:
  case RC65X_KEYSELECT:
  case RM_YD065_KEYPLAY:
  case RM_YD065_KEYOK:
  case RC65X_KEYUP: // Control wheel up
  case RM_YD065_KEYUP:
    Serial.print(F("Got remote \"P\"\n"));
    const uint16_t lightLevel = hw.photocell2();
    const TimeOfDay::DayPart dayPart = timeOfDay.updateAverage(lightLevel);
    setToMode(dayPart);
    light1.setOnLightLevel(ON);
    light5.setOnLightLevel(ON);
    light6.setOnLightLevel(ON);
    allLightsOn();
    break;
  }
}

void serialPrintCustomStatus()
{
  sprintf(sprintfBuffer,
          "|1:%1i:%3d|2:%3d|3:%3d|4:%3d|5:%1i:%3d|6:%1i:%3d|7:%3d,r:%3d|b:%3d|",
          int(light1.getLightMode()), hw.o1,
          hw.o2,
          hw.o3,
          hw.o4,
          int(light5.getLightMode()), hw.o5,
          int(light6.getLightMode()), hw.o6,
          hw.o7,
          hw.o13,hw.o8);
  
  Serial.print(sprintfBuffer);
}

void setupLightingAndMotorChannels()
{
  light1.setup(hw.o1, ON);
  light5.setup(hw.o5, ON);
  light6.setup(hw.o6, ON);
}
