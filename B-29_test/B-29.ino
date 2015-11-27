#include "lucky7.h"
#include "RC65X.h"
#include "RMYD065.h"
//#include "IRremote.h"
#include <EEPROM.h>

// There's an interrupt collision with the IR routines and the PWM
// Outputs 1, 5, 6 can be dimmed
// Outputs 2, 3, 7 cannont be dimmed
// Output 4, 8 and 13 are digital, cannot be dimmed
//
// Output 8 is the red led on the board
// Output 13 is the blue led on the board


// Output mapping to lights/motors
// Lights:
// 
//  01 Identification:
//     Mid-Fuselete Bottom Identification (3)
// 
//  05 Position
//     Wing Tips (2)
//     Tail (1)
// 
//  06 Formation:
//     Wing Top (6)
//     Fuselage Top (3)
// 
//  04 Illumination:
//     Wheel Wells (3)
// 
//  02 Landing:
//     Wing Bottom Retractable Landing Lights (2)
// 
// Motors:
//  03 Retractable Landing Light Motor Up
//     Motor Up (1)
//  07 Retractable Landing Light Motor Down
//     Motor Down (1)
// 
// Others:
//  No channel needed yet:
//     Interior
//     Tail Illumination

enum {
    LIGHTTHRESHOLDADDRESSH,
    LIGHTTHRESHOLDADDRESSL
} ;

enum {
    MODE_OVERRIDE   = 'O',
    MODE_BATTERYLOW = 'B',
    MODE_EVENING    = 'E',
    MODE_NIGHT      = 'N',
    MODE_PREDAWN    = 'P',
    MODE_MORNING    = 'M',
    MODE_DAY        = 'D',
} ;

enum {
    COLL_ON1,
    COLL_OFF1,
    COLL_ON2,
    COLL_OFF2
} ;

#define TIMEOUTSTATUS          1000        //   1 sec in milliseconds
#define TIMEOUTEVENING         14400000U   //   4 hours in milliseconds

#define TIMEOUTOVERRIDE        600000      //  10 minutes

#define TIMEOUTCOLLISIONON     50          // .05 sec
#define TIMEOUTCOLLISIONSHORT  250         // .25 sec
#define TIMEOUTCOLLISIONLONG   2100        // 2.1 sec
#define TIMEOUTTAXI            300000      //   5 minutes
#define TIMEOUTBATTERYLOW      30000       //  30 sec
#define TIMEOUTBATTERYLOWFLASH 100         //  .1 sec
#define BATTERYLOW             11.8        // Volts  
#define MILLISIN30DAYS         2592000000U //  30 days
 
uint32_t timeoutStatus;
uint16_t lightThreshold;
uint32_t timeoutBatteryLow;
uint32_t timeoutOverride;

uint8_t  mode;
uint16_t photocellLevel;

Lucky7      hw          = Lucky7();
UpDownMotor upDownMotor = UpDownMotor();
TimeOfDay   timeOfDay   = TimeOfDay(); 

void resetTimeoutBatteryLow() {
  timeoutBatteryLow = millis() + TIMEOUTBATTERYLOW;
}

bool overrideBatteryLow() {
  // If either of these is on, battery may read as low, so don't check battery voltage.
  return hw.o3 || hw.o7;
}

void resetTimeoutOverride() {
  timeoutOverride   = millis() + TIMEOUTOVERRIDE;
}

void idenOn() {hw.o1On ();};
void landOn() {hw.o2On ();};
void illuOn() {hw.o4On ();};
void posiOn() {hw.o5On ();};
void formOn() {hw.o6On ();};
void blueOn() {hw.o8On ();};
void redOn () {hw.o13On();};

void idenOff() {hw.o1Off ();};
void landOff() {hw.o2Off ();};
void illuOff() {hw.o4Off ();};
void posiOff() {hw.o5Off ();};
void formOff() {hw.o6Off ();};
void blueOff() {hw.o8Off ();};
void redOff () {hw.o13Off();};

void allLightsOn() {
  idenOn();
  landOn();
  illuOn();
  posiOn();
  formOn();
}

void allLightsOff() {
  idenOff();
  landOff();
  illuOff();
  posiOff();
  formOff();
}

void allOff() {
  upDownMotor.motorStop();
  allLightsOff();
}

void setLightsEvening() {
  allLightsOn();
}

void setLightsNight() {
  allLightsOn();
}

void setLightsPredawn() {
  allLightsOn();
}

void setLightsMorning() {
  allLightsOn();
}

void setLightsDay() {
  allLightsOn();
}

void setToMode( int targetMode) {
  switch (targetMode) {
  case MODE_OVERRIDE:
    mode = MODE_OVERRIDE;
    resetTimeoutOverride();
    break;
  case MODE_BATTERYLOW:
    mode = MODE_BATTERYLOW;
    resetTimeoutBatteryLow();
    if (mode != MODE_BATTERYLOW) {
      upDownMotor.motorStop();            
    }
    break;
  case MODE_EVENING:
    mode = MODE_EVENING;
    break;
  case MODE_NIGHT:
    mode = MODE_NIGHT;
    break;
  case MODE_PREDAWN:
    mode = MODE_PREDAWN;
    break;
  case MODE_MORNING:
    mode = MODE_MORNING;
    break;
  case MODE_DAY:
    mode = MODE_DAY;
    break;
  }
}

void processKey(uint32_t key) {
    Serial.print("key ");
    Serial.println(key, HEX);
    switch (key) {
      case '0':
      case RC65X_KEY0:
      case RC65X_KEYDOWN: // Control wheel down
      case RM_YD065_KEY0:
      case RM_YD065_KEYDOWN:
          allOff();
          break;
      case '1':
      case RC65X_KEY1:
      case RM_YD065_KEY1:
          hw.o1Toggle();
          break;
      case '2':
      case RC65X_KEY2:
      case RM_YD065_KEY2:
          hw.o2Toggle();
          break;
//    case '3':
//    case RC65X_KEY3:
//    case RM_YD065_KEY3:
//        hw.o3Toggle();
//        break;
      case '4':
      case RC65X_KEY4:
      case RM_YD065_KEY4:
          hw.o4Toggle();
          break;
      case '5':
      case RC65X_KEY5:
      case RM_YD065_KEY5:
          hw.o5Toggle();
          break;
      case '6':
      case RC65X_KEY6:
      case RM_YD065_KEY6:
          hw.o6Toggle();
          break;
//    case '7':
//    case RC65X_KEY7:
//    case RM_YD065_KEY7:
//        hw.o7Toggle();
//        break;
      case 'B':
      case RC65X_KEYRED:
      case RM_YD065_KEYRED:
          setToMode(MODE_BATTERYLOW);
          break;
      case 'R': // Re-read photocell values?  Reset photocell values?  Reset 
      case RC65X_KEYTVINPUT:
      case RM_YD065_KEYINPUT:
          uint16_t val;
          val = hw.photocell2();
          EEPROM.write(LIGHTTHRESHOLDADDRESSH, (val >> 8));
          EEPROM.write(LIGHTTHRESHOLDADDRESSL, (val & 0xFF));
          lightThreshold = (EEPROM.read(LIGHTTHRESHOLDADDRESSH) << 8) + EEPROM.read(LIGHTTHRESHOLDADDRESSL);
          break;
      case '8':
      case RC65X_KEY8:
      case RC65X_KEYUP: // Control wheel up
      case RM_YD065_KEY8:
      case RM_YD065_KEYUP:
          allLightsOn();
          break;
      case 'U':
      case RC65X_KEYCHANUP:
      case RM_YD065_KEYVOLUMEUP:
      case RM_YD065_KEYPROGUP:
          Serial.print("Got remote \"U\"\n");
          upDownMotor.motorUpStart();
          break;
      case 'D':
      case RC65X_KEYCHANDOWN:
      case RM_YD065_KEYVOLUMEDOWN:
      case RM_YD065_KEYPROGDOWN:
          Serial.print("Got remote \"D\"\n");
          upDownMotor.motorDownStart();
          break;
      }
}

void updateLights() {

    switch (mode) {
    case MODE_OVERRIDE:
      hw.boardLight(Lucky7::LIGHT_FAST_BLINK, redOn ,redOff );
      hw.boardLight(Lucky7::LIGHT_FAST_BLINK, blueOn,blueOff);
      upDownMotor.motorUpdate();
      break;
    case MODE_BATTERYLOW:
      allOff();
      hw.boardLight(Lucky7::LIGHT_FAST_BLINK, redOn, redOff );
      hw.boardLight(Lucky7::LIGHT_OFF, blueOn, blueOff);
      break;
    }
}

void statemap() {
  float batteryVoltage = hw.batteryVoltage();
  if ((mode != MODE_BATTERYLOW) && ! overrideBatteryLow() && batteryVoltage <= BATTERYLOW) {
    setToMode(MODE_BATTERYLOW);
  }

  const uint16_t lightLevel = hw.photocell2();
  TimeOfDay::DayPart dayPart = timeOfDay.updateAverage(lightLevel);

  switch (mode) {
  case MODE_BATTERYLOW:
    if (millis() > timeoutBatteryLow) {
      if (batteryVoltage <= BATTERYLOW) {
        resetTimeoutBatteryLow();
      } else {
        timeoutBatteryLow = 0;
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
    char buffer[40];
    if (millis() > timeoutStatus) {
        // Serial.print("\x1B[0;0f\x1B[K"); // home
        // Serial.print("\x1B[0J"); // clear

        Serial.print(millis());

        timeoutStatus = millis() + TIMEOUTSTATUS;
        Serial.print(" lightLevel:");
        Serial.print(hw.photocell2());
        Serial.print(" ");
        Serial.print("lightThreshold:");
        Serial.print(lightThreshold);
        Serial.print(" ");
        Serial.print("mode:");
        Serial.print((char)mode);
        if (mode == MODE_BATTERYLOW) {
          Serial.print(" timeoutBatteryLow: ");
          Serial.print(timeoutBatteryLow);
        }
        sprintf(buffer," output:%3d %3d %3d %3d %3d %3d %3d",hw.o1,hw.o2,hw.o3,hw.o4,hw.o5,hw.o6,hw.o7);
        Serial.print(buffer);

        Serial.print(" ");
        Serial.print("volt:");
        Serial.print(hw.batteryVoltage(),2);

        Serial.println();
    }

    // Reset after 30 days continuous running
    if (millis() > MILLISIN30DAYS) {
       asm volatile ("  jmp 0"); 
    }
}

void input() {
    unsigned long irKey;
    irKey = hw.loop();
    if (irKey) {
        setToMode(MODE_OVERRIDE);
        processKey(irKey);
    }

    if (Serial.available()) {
        setToMode(MODE_OVERRIDE);
        processKey(Serial.read()); 
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("NMNSH B-29 Lighting Controller setup");

    hw.setup(); // Currently zeros out everything, and initializes some stuff.
    upDownMotor.setup(&hw.o3, &hw.o7); // Initialize with (up, down) outputs
    timeOfDay.setup(500,500,10); // photocell value min, max and night/day threshhold %
      
    lightThreshold = (EEPROM.read(LIGHTTHRESHOLDADDRESSH) << 8) + EEPROM.read(LIGHTTHRESHOLDADDRESSL);
    lightThreshold = 100;

    timeoutStatus = 0;
    timeoutOverride = 0;
    timeoutBatteryLow = 2000; // Initial timeout is 2 seconds to let levels settle down but statup almost right away.


    setToMode(MODE_DAY);
}

void loop() {
    input();        // 1) Call hw.loop() and ask Lucky7 hardware to set all
                    //    hardware output levels based on software output 
                    //    levels (0-255)
                    // 2) Read remote control and serial port. If find something:
                    //    2a) Put in MODE_OVERRIDE if find something on either, 
                    //        remote control has precedence
                    //    2b) Call procdss key to set MODE_* and optionally set 
                    //        some output levels (0-255) and/or (re)set timers 
                    //        and/or re(set) eprom data (like photo sensor
                    //        threshold)
    statemap();     // 1) Read battery, MODE_BATTERYLOW if low
                    // 2) Walk through statemap using case on MODE_* and timers
                    //    2a) Change MODE_* as needed based on timers and sensor
                    //        thresholds.
                    //    2b) Maybe initiallize some levels and times.
    updateLights(); // Set the software output levels (0-255) on the outputs.
    status();       // Print to serial port, reset board if running for 30 days.

}

