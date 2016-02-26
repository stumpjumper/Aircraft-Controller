#include "lucky7.h"
#include "RC65X.h"
#include "RMYD065.h"
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
//  01 Identification (d):
//     Mid-Fuselete Bottom Identification (3)
// 
//  05 Position (nd):
//     Wing Tips (2)
//     Tail (1)
// 
//  06 Formation (d):
//     Wing Top (6)
//     Fuselage Top (3)
// 
//  04 Illumination (nd):
//     Wheel Wells (3)
// 
//  02 Landing (nd):
//     Wing Bottom Retractable Landing Lights (2)
// 
// Motors:
//  03 Retractable Landing Light Motor Up (nd)
//     Motor Up (1)
//  07 Retractable Landing Light Motor Down (nd)
//     Motor Down (1)
// 
// Others:
//  No channel needed yet:
//     Interior
//     Tail Illumination

#ifdef DOING_UNIT_TESTING
  uint32_t myIRKey = 0;
#endif

enum Mode {
  MODE_NOTSET     = 0,
  MODE_OVERRIDE   = 'O',
  MODE_BATTERYLOW = 'B',
  MODE_EVENING    = 'E',
  MODE_NIGHT      = 'N',
  MODE_PREDAWN    = 'P',
  MODE_MORNING    = 'M',
  MODE_DAY        = 'D',
} ;

enum Coll {
  COLL_ON1,
  COLL_OFF1,
  COLL_ON2,
  COLL_OFF2
} ;

#define TIMEOUTSTATUS              1000  //   1 sec in milliseconds
#define TIMEOUTEVENING         18000000U //   5 hours in milliseconds

#define TIMEOUTOVERRIDE          300000  //   5 minutes

#define TIMEOUTCOLLISIONON           50  // .05 sec
#define TIMEOUTCOLLISIONSHORT       250  // .25 sec
#define TIMEOUTCOLLISIONLONG       2100  // 2.1 sec
#define TIMEOUTBATTERYLOW         30000  //  30 sec
#define TIMEOUTBATTERYLOWFLASH      100  //  .1 sec
#define BATTERYLOW                 11.8  // Volts  
#define MILLISIN30DAYS       2592000000U //  30 days
//                        2,592,000,000
 
uint32_t timeoutStatus = 0;
uint32_t timeoutBatteryLow = 0;
uint32_t timeoutOverride = 0;
uint32_t timeoutUpdateLights = 0;

uint8_t  mode = MODE_NOTSET;

Lucky7      hw          = Lucky7();
TimeOfDay   timeOfDay   = TimeOfDay(); 
UpDownMotor upDownMotor = UpDownMotor();

// Decay settings for position lights
// Note: Added .01 sec to on & decay lengths so will not be in sync with F-16
uint32_t positionOnLengths[1]          =    {110}; // On for .11 seconds
uint32_t positionDecayLengths[1]       =   {1110}; // Decay for 1.11
uint8_t  positionMaxLightLevels[1]     =     {ON}; // Full on when on  
uint32_t positionTauInMilliseconds[1]  =    {175}; // Half-life = .05 seconds

// Decay settings for taxi lights during day and night
uint32_t   landingDayOnLengths[1]      = {300000}; // On 5 minutes
uint32_t   landingDayDecayLengths[1]   = {300000}; // Off for 5 minutes
uint8_t    landingDayMaxLightLevels[1] =     {ON}; // Full on when on  
uint32_t * landingDayTauInMilliseconds =     NULL; // On/Off, no decay

// Light objects to control each channel
Light      ident    ; // Identification: Mid-Fuselete Bottom Identification (3)
DecayLight position ; // Position      : Wing Tips (2), Tail (1)
Light      formation; // Formation     : Wing Top (6), Fuselage Top (3)
DecayLight landing  ; // Landing       : Wing Bot Retractable Landing Lights (2)
Light      illum    ; // Illumination  : Wheel Wells (3)
// Light objects to control each status light on the board
FastSlowBlinkingLight blueLight; // Blue light on Aurdino board
FastSlowBlinkingLight redLight ; // Red light on Aurdino board

void resetTimeoutBatteryLow() {
  const uint32_t time = millis();
  const uint32_t timeoutBatteryLowOld = timeoutBatteryLow;
  timeoutBatteryLow = time + TIMEOUTBATTERYLOW;
  if (timeoutBatteryLow < timeoutBatteryLowOld) {
    Serial.print(F("ERROR: Detected uint32_t overflow in resetTimeoutBatteryLow()\n"));
  }
}

bool overrideBatteryLow() {
  // If either of these is on, battery may read as low, so don't check battery voltage.
  return hw.o3 || hw.o7;
}

void resetTimeoutOverride() {
  const uint32_t time = millis();
  const uint32_t timeoutOverrideOld = timeoutOverride;
  timeoutOverride   = time + TIMEOUTOVERRIDE;
  if (timeoutOverride < timeoutOverrideOld) {
    Serial.print(F("ERROR: Detected uint32_t overflow in resetTimeoutOverride()\n"));
  }
}

void allLightsOn() {
  ident.on();
  landing.on();
  illum.on();
  position.on();
  formation.on();
}

void allLightsOff() {
  ident.off();
  landing.off();
  illum.off();
  position.off();
  formation.off();
}

void updateLights() {
  // no need to hammer this
  const uint32_t time = millis();
  if (time > timeoutUpdateLights) {
    timeoutUpdateLights = time + 10;

    ident.update();
    landing.update();
    illum.update();
    position.update();
    formation.update();
    upDownMotor.motorUpdate();
    
    blueLight.update();
    redLight.update();
  }
}

void allOff() {
  upDownMotor.motorStop();
  allLightsOff();
}

//         Mode              Red          Blue
// ---------------------  ----------   ----------
// MODE_OVERRIDE   = 'O'  slow blink   fast blink
// MODE_BATTERYLOW = 'B'  fast blink   slow blink
// MODE_EVENING    = 'E'  off          slow blink
// MODE_NIGHT      = 'N'  off          on
// MODE_PREDAWN    = 'P'  slow blink   on
// MODE_MORNING    = 'M'  slow blink   off
// MODE_DAY        = 'D'  on           off

void setOverride() {
  redLight .setToSlow();
  redLight .resume();
  blueLight.setToFast();
  blueLight.resume();

  allOff();
}

void setBatteryLow() {
  redLight .setToFast();
  redLight .resume();
  blueLight.setToSlow();
  blueLight.resume();

  allOff();
}

void setEvening() {
  redLight .off();
  blueLight.setToSlow();
  blueLight.resume();

  ident.on();
  landing.on();
  illum.on();
  position.resume();
  formation.on();
}

void setNight() {
  redLight .off();
  blueLight.on();
  
  allOff();
}

void setPreDawn() {
  redLight .setToSlow();
  redLight .resume();
  blueLight.on();

  ident.on();
  landing.on();
  illum.on();
  position.resume();
  formation.on();
}

void setMorning() {
  redLight .setToSlow();
  redLight .resume();
  blueLight.off();

  ident.on();
  landing.on();
  illum.on();
  position.resume();
  formation.on();
}

void setDay() {
  redLight  .on();
  blueLight.off();

  ident.on();
  landing.resume();
  illum.off();
  position.resume();
  formation.on();
}

void setToMode( int targetMode) {

  switch (targetMode) {
  case MODE_OVERRIDE:
    if (MODE_OVERRIDE != mode) {
      setOverride();
    }
    mode = MODE_OVERRIDE;

    resetTimeoutOverride();
    break;
  case MODE_BATTERYLOW:
    if (MODE_BATTERYLOW != mode) {
      setBatteryLow();
    }
    mode = MODE_BATTERYLOW;

    resetTimeoutBatteryLow();
    break;
  case MODE_EVENING:
    if (MODE_EVENING != mode) {
      setEvening();
    }
    mode = MODE_EVENING;
    break;
  case MODE_NIGHT:
    if (MODE_NIGHT != mode) {
      setNight();
    }
    mode = MODE_NIGHT;
    break;
  case MODE_PREDAWN:
    if (MODE_PREDAWN != mode) {
      setPreDawn();
    }
    mode = MODE_PREDAWN;
    break;
  case MODE_MORNING:
    if (MODE_MORNING != mode) {
      setMorning();
    }
    mode = MODE_MORNING;
    break;
  case MODE_DAY:
    if (MODE_DAY != mode) {
      setDay();
    }
    mode = MODE_DAY;
    break;
  }
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
    ident.toggle();
    break;
  case '2':
  case RC65X_KEY2:
  case RM_YD065_KEY2:
    Serial.print(F("Got remote \"2\"\n"));
    setToMode(MODE_OVERRIDE);
    landing.toggle();
    break;
//    case '3':
//    case RC65X_KEY3:
//    case RM_YD065_KEY3:
//        hw.o3Toggle();
//        break;
  case '4':
  case RC65X_KEY4:
  case RM_YD065_KEY4:
    Serial.print(F("Got remote \"4\"\n"));
    setToMode(MODE_OVERRIDE);
    illum.toggle();
    break;
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
    formation.toggle();
    break;
//    case '7':
//    case RC65X_KEY7:
//    case RM_YD065_KEY7:
//        hw.o7Toggle();
//        break;
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
    upDownMotor.motorUpStart();
    break;
  case 'D':
  case RC65X_KEYCHANDOWN:
  case RM_YD065_KEYVOLUMEDOWN:
  case RM_YD065_KEYPROGDOWN:
    Serial.print(F("Got remote \"D\"\n"));
    upDownMotor.motorDownStart();
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

void statemap() {
  const float batteryVoltage = hw.batteryVoltage();
  if ((mode != MODE_BATTERYLOW) &&
      ! overrideBatteryLow()    &&
      batteryVoltage <= BATTERYLOW) {
    setToMode(MODE_BATTERYLOW);
  }

  const uint16_t lightLevel = hw.photocell2();
  const TimeOfDay::DayPart dayPart = timeOfDay.updateAverage(lightLevel);

  switch (mode) {
  case MODE_BATTERYLOW:
    if (millis() > timeoutBatteryLow) {
      if (batteryVoltage <= BATTERYLOW) {
        resetTimeoutBatteryLow();
      } else {
        setToMode(dayPart);
      }
    }
    break;
  case MODE_OVERRIDE:
    if (millis() > timeoutOverride) {
      setToMode(dayPart);
    }
    break;
  default:
    setToMode(dayPart);
    break;
  }
}

void status() {
    char buffer[75];
    const uint32_t time = millis();
    if (time > timeoutStatus) {
        timeoutStatus = time + TIMEOUTSTATUS;
        // Serial.print(F("\x1B[0;0f\x1B[K")); // home
        // Serial.print(F("\x1B[0J")); // clear

        Serial.print(time);
        Serial.print(F(" p:"));
        Serial.print(hw.photocell2());
        Serial.print(F(" pMx:"));
        Serial.print(timeOfDay.getPhotocellAvgValueMax());
        Serial.print(F(" pMn:"));
        Serial.print(timeOfDay.getPhotocellAvgValueMin());
        Serial.print(F(" pAv:"));
        Serial.print(timeOfDay.getPhotocellAvgValueCurrent());
        Serial.print(F(" lN:"));
        Serial.print(timeOfDay.getLengthOfNight()/3600000);
        Serial.print(F(" m:"));
        Serial.print((char)mode);
        if (mode == MODE_BATTERYLOW) {
          Serial.print(F(" tBL: "));
          Serial.print(uint32_t(timeoutBatteryLow-time));
        }
        if (mode == MODE_OVERRIDE) {
          Serial.print(F(" tOr: "));
          Serial.print(uint32_t(timeoutOverride)-time);
        }
        sprintf(buffer,
                "|1:%1i:%3d|2:%1i:%3d|3:%3d|4:%1i:%3d|5:%1i:%3d|6:%1i:%3d|7:%3d,r:%3d|b:%3d|",
                int(ident.getLightMode()), hw.o1,
                int(landing.getLightMode()), hw.o2,
                hw.o3,
                int(illum.getLightMode()), hw.o4,
                int(position.getLightMode()), hw.o5,
                int(formation.getLightMode()), hw.o6,
                hw.o7,
                hw.o13,hw.o8);

        Serial.print(buffer);

        Serial.print(F("r:"));
        Serial.print(redLight.getLightMode());
        Serial.print(F(" b:"));
        Serial.print(blueLight.getLightMode());

        Serial.print(F(" v:"));
        Serial.print(hw.batteryVoltage(),2);

        Serial.println();
    }

    // Reset after 30 days continuous running
    if (time > MILLISIN30DAYS) {
      asm volatile ("  jmp 0"); 
    }
}

void input() {
    uint32_t irKey;
    irKey = hw.loop();
#ifdef DOING_UNIT_TESTING
    if (myIRKey != 0) {
      irKey = myIRKey;
    }
#endif
    if (irKey) {
      processKey(irKey);
    }

    if (Serial.available()) {
      processKey(Serial.read()); 
    }
}

void setupLightingAndMotorChannels()
{
  ident     .setup(hw.o1, ON);
  landing   .setup(hw.o2, ON, 1, landingDayOnLengths, landingDayDecayLengths,
                   landingDayMaxLightLevels, landingDayTauInMilliseconds);
  illum     .setup(hw.o4, ON);
  position  .setup(hw.o5, ON, 1, positionOnLengths, positionDecayLengths,
                   positionMaxLightLevels, positionTauInMilliseconds);
  formation .setup(hw.o6, ON);

  upDownMotor.setup(hw.o3, hw.o7); // Initialize with (up, down) outputs

  blueLight .setup(hw.o8 , ON, ON);
  redLight  .setup(hw.o13, ON, ON);

}
void setup() {
    Serial.begin(115200);
    Serial.println(F("NMNSH B-29 Lighting Controller setup"));

    hw.setup(); // Currently zeros out everything, and initializes some stuff.
    timeOfDay.setup(500,500,10); // photocell value min, max and night/day threshhold %

    setupLightingAndMotorChannels();
      
    timeoutStatus = 0;
    timeoutOverride = 0;
    timeoutUpdateLights = 0;
    timeoutBatteryLow = 2000; 

    // Initial put in battery low mode to let levels settle down
    // but set timout short so will statup almost right away.
    timeoutBatteryLow = 2000; // 2 Seconds to "warm up"
    setBatteryLow();
    mode = MODE_BATTERYLOW;
}

void loop() {
    input();        // 1) Call hw.loop() and ask Lucky7 hardware to set all
                    //    hardware output levels based on software output 
                    //    levels (0-255)
                    // 2) Read IR and serial port. If find something:
                    //    2a) Put in MODE_OVERRIDE if find something on either,
                    //        IR port or serial port, IR has precedence
                    //    2b) Call processKey to set correct MODE_* and
                    //        optionally set set output levels (On/OFF)
                    //        and/or (re)set timers.
    statemap();     // 1) Read battery and put in MODE_BATTERYLOW if needed
                    // 2) Get dayPart from TimeOfDay class.
                    //    Pass it the photocell level, which is used to figure
                    //    out what the time of day is
                    // 3) If MODE_BATTERYLOW or MODE_OVERRIDE, see if should
                    //    exit these based on new battery level and/or
                    //    timeout counters.  Reset counters as needed.
                    //    3a) If exit, change MODE_* as needed based on
                    //        setToMode(dayPart)
                    // 4) If not MODE_BATTERYLOW or MODE_OVERRIDE, 
                    //    simply call setToMode(dayPart)
    updateLights(); // Loop over the Light objects and call update on them,
                    // but only do this every 10 millis, not every 1 millis
    status();       // Print to serial port, reset board if running for 30 days.
}

