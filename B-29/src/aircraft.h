#ifndef AIRCRAFT_H
#define AIRCRAFT_H
#include "lucky7.h"
#include "RC65X.h"
#include "RMYD065.h"

void serialPrintBanner();
void setOverride();
void setBatteryLow();
void allOff();
void setEvening();
void setNight();
void setPreDawn();
void setMorning();
void setDay();
bool overrideBatteryLow();
void processKey(uint32_t irKey);
void setupLightingAndMotorChannels();
void setBatteryLow();
void updateAll();
void serialPrintCustomStatus();

//         Mode              Red          Blue
// ---------------------  ----------   ----------
// MODE_OVERRIDE   = 'O'  slow blink   slow blink
// MODE_BATTERYLOW = 'B'  fast blink   fast blink
// MODE_EVENING    = 'E'  off          slow blink
// MODE_NIGHT      = 'N'  off          on
// MODE_PREDAWN    = 'P'  slow blink   on
// MODE_MORNING    = 'M'  slow blink   off
// MODE_DAY        = 'D'  on           off

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

#define TIMEOUTSTATUS              1000  //   1 sec in milliseconds
#define TIMEOUTEVENING         18000000U //   5 hours in milliseconds

#define TIMEOUTOVERRIDE          300000  //   5 minutes

#define TIMEOUTBATTERYLOW         30000  //  30 sec
#define TIMEOUTBATTERYLOWFLASH      100  //  .1 sec
#define BATTERYLOW                 11.8  // Volts  
#define MILLISIN30DAYS       2592000000U //  30 days
//                        2,592,000,000
 
uint32_t timeoutStatus = 0;
uint32_t timeoutBatteryLow = 0;
uint32_t timeoutOverride = 0;
uint32_t timeoutUpdateLights = 0;

char sprintfBuffer[75]; // Buffer to put sprintf text into

uint8_t  mode = MODE_NOTSET;

Lucky7    hw        = Lucky7();
TimeOfDay timeOfDay = TimeOfDay(); 

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

void resetTimeoutOverride() {
  const uint32_t time = millis();
  const uint32_t timeoutOverrideOld = timeoutOverride;
  timeoutOverride   = time + TIMEOUTOVERRIDE;
  if (timeoutOverride < timeoutOverrideOld) {
    Serial.print(F("ERROR: Detected uint32_t overflow in resetTimeoutOverride()\n"));
  }
}

void setOverride() {
  Serial.println(F("In setOverride()"));
  redLight .setToSlow();
  redLight .flash();
  blueLight.setToSlow();
  blueLight.flash();

  allOff();
}

void setBatteryLow() {
  redLight .setToFast();
  redLight .flash();
  blueLight.setToFast();
  blueLight.flash();
  
  allOff();
}

void setEveningInit() {
  redLight .off();
  blueLight.setToSlow();
  blueLight.flash();
  
  setEvening();
}

void setNightInit() {
  redLight .off();
  blueLight.on();
  
  setNight();
}

void setPreDawnInit() {
  redLight .setToSlow();
  redLight .flash();
  blueLight.on();
  
  setPreDawn();
}

void setMorningInit() {
  redLight .setToSlow();
  redLight .flash();
  blueLight.off();
  
  setMorning();
}

void setDayInit() {
  redLight  .on();
  blueLight.off();
  
  setDay();
}

void updateAllInit() {
  blueLight.update();
  redLight .update();

  updateAll();
}

void updateChannels() {
  // no need to hammer this
  const uint32_t time = millis();
  if (time > timeoutUpdateLights) {
    timeoutUpdateLights = time + 10;
    
    updateAllInit();
  }
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
      setEveningInit();
    }
    mode = MODE_EVENING;
    break;
  case MODE_NIGHT:
    if (MODE_NIGHT != mode) {
      setNightInit();
    }
    mode = MODE_NIGHT;
    break;
  case MODE_PREDAWN:
    if (MODE_PREDAWN != mode) {
      setPreDawnInit();
    }
    mode = MODE_PREDAWN;
    break;
  case MODE_MORNING:
    if (MODE_MORNING != mode) {
      setMorningInit();
    }
    mode = MODE_MORNING;
    break;
  case MODE_DAY:
    if (MODE_DAY != mode) {
      setDayInit();
    }
    mode = MODE_DAY;
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

void status() {
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

        serialPrintCustomStatus();        

        Serial.print(F(" r:"));
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

void setupStatusLights() {
  blueLight.setup(hw.o8 , ON, ON); // Setup status light blue
  redLight .setup(hw.o13, ON, ON); // and red
}

void serialPrintTimeStamp ()
{
  #ifndef DOING_UNIT_TESTING
  Serial.print(F(__DATE__));
  Serial.print(F(" "));
  Serial.println(F(__TIME__));
  #endif
}

void setup() {
    Serial.begin(115200);
    serialPrintTimeStamp();
    serialPrintBanner();
    hw.setup(); // Currently zeros out everything, and initializes some stuff.
    timeOfDay.setup(500,500,10); // photocell value min, max and night/day threshhold %

    setupStatusLights();
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
    updateChannels();//Loop over the Light objects and call update on them,
                    // but only do this every 10 millis, not every 1 millis
    status();       // Print to serial port, reset board if running for 30 days.
}
#endif // AIRCRAFT_H
