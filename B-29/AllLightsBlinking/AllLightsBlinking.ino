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
SlowBlinkingLight  light1; 
SlowBlinkingLight  light2;
SlowBlinkingLight  light3;
SlowBlinkingLight  light4;
SlowBlinkingLight  light5;
SlowBlinkingLight  light6;
SlowBlinkingLight  light7;

void serialPrintBanner() {
    Serial.println(F("AllLightsBlinking Lights"));
}

bool overrideBatteryLow() {
  // Not used for these lights.  See B-29 for sample usage
  return false;
}

void allLightsOn() {
  Serial.println(F("In allLightsOn()"));
  light1.on();
  light2.on();
  light3.on();
  light4.on();
  light5.on();
  light6.on();
  light7.on();
}

void allLightsOff() {
  Serial.println(F("In allLightsOff()"));
  light1.off();
  light2.off();
  light3.off();
  light4.off();
  light5.off();
  light6.off();
  light7.off();
}

void updateAll() {
  light1.update();
  light2.update();
  light3.update();
  light4.update();
  light5.update();
  light6.update();
  light7.update();
}

void allLightsFlash() {
  light1.flash();
  light2.flash();
  light3.flash();
  light4.flash();
  light5.flash();
  light6.flash();
  light7.flash();
}


void allOff() {
  Serial.println(F("In allOff()"));
  allLightsOff();
}

// -------------------- Time of Day Settings ----------------
void setEvening() {
  allLightsFlash();
}

void setNight() {
  allLightsFlash();
}

void setPreDawn() {
  allLightsFlash();
}

void setMorning() {
  allLightsFlash();
}

void setDay() {
  allLightsFlash();
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
  case 'U':
  case RC65X_KEYCHANUP:
  case RM_YD065_KEYVOLUMEUP:
  case RM_YD065_KEYPROGUP:
    Serial.print(F("Got remote \"U\"\n"));
    setToMode(MODE_OVERRIDE);
    allLightsOn();
    break;
  case 'D':
  case RC65X_KEYCHANDOWN:
  case RM_YD065_KEYVOLUMEDOWN:
  case RM_YD065_KEYPROGDOWN:
    Serial.print(F("Got remote \"D\"\n"));
    setToMode(MODE_OVERRIDE);
    allLightsOff();
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
          "|1:%1i:%3d|2:%3d|3:%3d|4:%3d|5:%3d|6:%3d|7:%3d,r:%3d|b:%3d|",
          int(light1.getLightMode()), hw.o1,
          hw.o2,
          hw.o3,
          hw.o4,
          hw.o5,
          hw.o6,
          hw.o7,
          hw.o13,hw.o8);
  
  Serial.print(buffer);
}

void setupLightingAndMotorChannels()
{
  light1.setup(hw.o1, ON, ON);
  light2.setup(hw.o2, ON, ON);
  light3.setup(hw.o3, ON, ON);
  light4.setup(hw.o4, ON, ON);
  light5.setup(hw.o5, ON, ON);
  light6.setup(hw.o6, ON, ON);
  light7.setup(hw.o7, ON, ON);
}
