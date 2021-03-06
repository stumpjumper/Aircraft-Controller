#ifndef LUCKY7_H
#define LUCKY7_H
#include "Arduino.h"
#ifndef DOING_UNIT_TESTING
 #define FRIEND_TEST(a, b)
#else
 #include "Serial.h"
#endif

#define OFF 0
#define ON  255

// PWM outputs
#define O1  11
#define O2  10
#define O3  9
#define O5  6
#define O6  5
#define O7  3

#define O4  7      
#define O8  8
#define O13 13

#define IR  2

#define AVECNT 10

// ((5/1024)/10000)*43000 ideal
// 12.16/553
#define BVSCALE 0.02198915009


#define LUCKY7_TIMEOUTMOTORUPDOWN    45000  // 45 sec in milliseconds
#define LUCKY7_TIMEMOTORDELAY         2000  // 2 sec
#define LUCKY7_TIME30SEC             30000  // 30 sec
#define LUCKY7_TIME5MIN             300000  // 300 sec = 5 min
#define LUCKY7_TIME2HOUR           7200000U // 2 hours
#define LUCKY7_TIME4HOUR          14400000U // 4 hours
#define LUCKY7_TIME12HOUR         43200000U // 12 hours

class Lucky7;

// Light class hierarchy
// class Light
// class RotatingLight     : public Light

// class DecayLight        : public Light            
// class FlashingLight     : public DecayLight
// class BlinkingLight     : public FlashingLight
// class FastBlinkingLight : public BlinkingLight
// class SlowBlinkingLight : public BlinkingLight
// class FastSlowBlinkingLight
//   FastBlinkingLight fastLight;
//   SlowBlinkingLight slowLight;

class Light
{
  // Base class for lights, and one that just does On/Off.
public:
  enum MODE {
    LIGHT_MODE_NOTSET = -1,
    LIGHT_OFF,
    LIGHT_ON,
    LIGHT_FLASHING};

private:
  FRIEND_TEST(Light, Constructor);
  FRIEND_TEST(Light, SetOnLightLevel);
  FRIEND_TEST(Light, IncrementOnLightLevel);
  FRIEND_TEST(Light, On);
  FRIEND_TEST(Light, Off);
  FRIEND_TEST(Light, FunctionCallOperatorGetValue);
  FRIEND_TEST(Light, FunctionCallOperatorSetValue);
  FRIEND_TEST(Light, Update);

  // Do not implement to make sure are never called
  Light(Light & other); 
  Light & operator=(const Light &rhs);

public:
  Light();
  virtual ~Light();

protected:
  uint8_t onLightLevel;  // Value when light simply On, not decaying or blinking
  uint8_t * p_lightLevel;
  Light::MODE lightMode;

public:

  void setup(uint8_t & lightLevelVariable, const uint8_t onLightLevelValue);
  void setOnLightLevel(const uint8_t onLightLevelValue);
  void incrementOnLightLevel(const int16_t onLightLevelIncrement);
  
  void update() {;};

  Light::MODE getLightMode() const {return lightMode;};
  
  void on() {*p_lightLevel = onLightLevel; lightMode = LIGHT_ON;};
  void off() {*p_lightLevel = OFF; lightMode = LIGHT_OFF;};
  void toggle();

  uint8_t & operator()(void) {return *p_lightLevel;};
  //  Light & operator=(const uint8_t value) {lightLevel = value; return *this;}
};


class RotatingLight: public Light
{
  // In this class the light level rises and then decays based on a sign wave such that
  // light level = (maxLightLevel-minLightLevel)*abs(sin(time*(Pi/pulseLength))) + minLightLevel


private:
  FRIEND_TEST(RotatingLight, Constructor);
  FRIEND_TEST(RotatingLight, Update);
  FRIEND_TEST(RotatingLight, Update2);
  FRIEND_TEST(RotatingLight, UpdateWithTauNULL);
  FRIEND_TEST(RotatingLight, UpdateCalledInfrequently);

protected:
  uint32_t flatLength;     // Time light is sitting at light level flatLightLevel, between sine-wave based pulses.
  uint8_t  flatLightLevel; // The light level where the light sits for time flatLength between pulses.
  uint32_t pulseLength;    // The half-period of the sine-wave. The total time the light is on and the value is first increasing then decreasing.
  uint8_t  minLightLevel;  // Value at which the pulsing light begins.
  uint8_t  maxLightLevel;  // Max value of the pulsing light.

  uint32_t changeTime;     // Keep track of when its time to change modes
  bool     pulsing;        // Flag if in pulse or non-pulse mode
  uint32_t pulseStartTime; // Keep track of when pulse started.

public:
  RotatingLight();
  ~RotatingLight();

  void setup(uint8_t & lightLevelVariable,    
             const uint8_t  onLightLevelValue,
             const uint32_t flatLengthValue,           
             const uint8_t  flatLightLevelValue,       
             const uint32_t pulseLengthValue,     
             const uint8_t  minLightLevelValue,
             const uint8_t  maxLightLevelValue);
  
  void flash() { on(); *p_lightLevel = minLightLevel; lightMode = LIGHT_FLASHING;}
  void update();
  bool getPulsing() {return pulsing;};
};


class DecayLight: public Light
{
  // A light that is on, then switches off, but filament "cools" when light
  // is switched off.
  // For each interval, specify length of time 'onLength' light is on at
  // 'maxLightLevel'.  Then, for each interval, specify 'decayLength' how long
  // light will spend in decay mode, which is either moving to off or off.
  // Length of time moving to off (i.e. decaying) is governed by 'tau' and
  // Newton's law of cooling, as desribed below.
  //
  // In this class the light level decays at a rate given by Newton's
  // law of cooling, which is
  // 
  // T = T_env + dT*exp(-t/tau)
  // where:
  //   t = time
  //   tau = time constant
  //   T = Temperature at time t
  //   T_env = Temperature of the surrounding environment
  //   dT = Initial temperature difference in bodies (T_0 - T_env)
  //   T_0 = Temperature at time 0.
  // 
  //   tau = ro*V*c_p/h*A_s
  //   ro = density
  //   V = volume
  //   c_p = heat capacity
  //   A_s = surface area
  //   h = heat transfer coefficient
  //
  // Note:
  // At t=0, T = T_0 = T_env + dT
  // At t=tau,   T will have decreased by 63.2% to .368*dT
  // At t=2*tau, T will have decreased by 86.5% to .135*dT  
  // 
  // For us T_env = 0 and dT = maxLightLevel so
  // light_level = maxLightLevel*exp(-millis()/(tau))
  // where tau is given in seconds.

private:
  FRIEND_TEST(DecayLight, Constructor);
  FRIEND_TEST(DecayLight, Update);
  FRIEND_TEST(DecayLight, Update2);
  FRIEND_TEST(DecayLight, UpdateWithTauNULL);
  FRIEND_TEST(DecayLight, UpdateCalledInfrequently);

protected:
  uint32_t changeTime;      // Keep track of when its time to change modes
  bool     decaying;        // Flag if in on or decay mode
  uint32_t decayStartTime;  // Keep track of when decay started.
  uint8_t  intervalIndex;   // Keep track of which lighting inverval we are on
  uint8_t  numIntervals;    // Length of onLenth, decayLength & maxLightLevels.

  uint32_t * onLength;      // Time to stay on before switching to decay mode,
                            // for each interval
  uint32_t * decayLength;   // Time to stay in decay mode, before starting next
                            // interval, for each interval
  uint8_t  * maxLightLevel; // Light level when on, for each interval
  uint32_t * tau;           // Time constants.  See discussion above


  
public:
  DecayLight();
  virtual ~DecayLight();

  void setup(uint8_t & lightLevelVariable,
             const uint8_t onLightLevel,
             const uint8_t numberOfValues,
             uint32_t * onLengthValues,
             uint32_t * decayLengthValues,
             uint8_t  * maxLightLevelValues,
             uint32_t * tauInMilliseconds);
  
  void flash() { on(); *p_lightLevel = maxLightLevel[intervalIndex]; lightMode = LIGHT_FLASHING;}
  void update();
  bool getDecaying() {return decaying;};
};

class FlashingLight : public DecayLight
{
  // Just like DecayLight but simply on and off, no decay.
  // This one takes arrays of onLength, offLength and maxLightLevels

private:
  FRIEND_TEST(FlashingLight, Constructor);

public:
  FlashingLight();
  virtual ~FlashingLight();

  void setup(uint8_t & lightLevelVariable,
             const uint8_t onLightLevel,
             const uint8_t numberOfValues,
             uint32_t * onLengthValues,
             uint32_t * offLengthValues,
             uint8_t  * maxLightLevelValues);
};

class BlinkingLight : public FlashingLight
{
  // Just like DecayLight but simply on and off with no decay
  // and one on and one off value are given.

private:
  FRIEND_TEST(BlinkingLight, Constructor);
  FRIEND_TEST(BlinkingLight, Update);
  FRIEND_TEST(FastBlinkingLight, Constructor);
  FRIEND_TEST(SlowBlinkingLight, Constructor);
  FRIEND_TEST(FastSlowBlinkingLight, Constructor);
  FRIEND_TEST(FastSlowBlinkingLight, SetToFast);
  FRIEND_TEST(FastSlowBlinkingLight, SetToSlow);
  FRIEND_TEST(FastSlowBlinkingLight, FunctionCallOperatorGetValue);
  FRIEND_TEST(B29Test, UpdateLights);

  uint32_t onLengthValues[1];
  uint32_t offLengthValues[1];
  uint8_t  maxLightLevelValues[1];
  
public:
  BlinkingLight();
  virtual ~BlinkingLight();
  void setup(uint8_t & lightLevelVariable,
             const uint8_t onLightLevel,
             const uint32_t onLengthValue,
             const uint32_t offLengthValue,
             const uint8_t  maxLightLevelValue);
};

class FastBlinkingLight : public BlinkingLight
{
  // A fast blinking light

public:                                        // 4 Hz
  static const uint32_t onLengthValue  = 125; // On for 1/8 of a second
  static const uint32_t offLengthValue = 125; // Off for 1/8 of a second
  
public:
  FastBlinkingLight();
  virtual ~FastBlinkingLight();
  void setup(uint8_t & lightLevelVariable,
             const uint8_t onLightLevel,
             const uint8_t maxLightLevelValue);
};

class SlowBlinkingLight : public BlinkingLight
{
  // A slow blinking light
  
public:
  static const uint32_t onLengthValue  = 1000; // On for .88 seconds
  static const uint32_t offLengthValue = 1000; // Off for .12 second
  
public:
  SlowBlinkingLight();
  virtual ~SlowBlinkingLight();
  void setup(uint8_t & lightLevelVariable,
             const uint8_t onLightLevel,
             const uint8_t maxLightLevelValue);
};

class FastSlowBlinkingLight
{
  // A fast or slow blinking light, use can choose.
  
public:
  enum Speed {
    NOTSET = 0,
    FAST,
    SLOW };
  
private:
  FRIEND_TEST(FastSlowBlinkingLight, Constructor);
  FRIEND_TEST(FastSlowBlinkingLight, FunctionCallOperatorGetValue);
  FRIEND_TEST(FastSlowBlinkingLight, SetToFast);
  FRIEND_TEST(FastSlowBlinkingLight, SetToSlow);
  FRIEND_TEST(B29Test, UpdateLights);
  
  // Do not implement to make sure are never called
  FastSlowBlinkingLight(Light & other); 
  FastSlowBlinkingLight & operator=(const Light &rhs);
  
  FastBlinkingLight fastLight;
  SlowBlinkingLight slowLight;
  BlinkingLight * p_currentLight;
  Speed blinkSpeed;
  
public:
  FastSlowBlinkingLight();
  ~FastSlowBlinkingLight();
  void setup(uint8_t & lightLevelVariable,
             const uint8_t onLightLevel,
             const uint8_t maxLightLevelValue);
  
  void setToFast() {p_currentLight = &fastLight; blinkSpeed = FAST;};
  void setToSlow() {p_currentLight = &slowLight; blinkSpeed = SLOW;};
  
  Light::MODE getLightMode() {return p_currentLight->getLightMode();};
  Speed getSpeed() {return blinkSpeed;};
  void on() {fastLight.on(); slowLight.on();};
  void off() {fastLight.off(); slowLight.off();};
  void flash() {fastLight.flash(); slowLight.flash();};
  void update() {p_currentLight->update();};
  
  uint8_t & operator()(void) {return (*p_currentLight)();};
  
  //  Light & operator=(const uint8_t value) {lightLevel = value; return *this;};
  
};

class TimeOfDay
{
private:
  FRIEND_TEST(TimeOfDayTest, setup);
  FRIEND_TEST(TimeOfDayTest, Constructor);
  FRIEND_TEST(TimeOfDayTest, getNightDayThreshold);
  FRIEND_TEST(TimeOfDayTest, UpdatePhotocellAvgValues);
  FRIEND_TEST(TimeOfDayTest, UpdateTimeOfDay);
  FRIEND_TEST(B29Test, Statemap);
  FRIEND_TEST(B29Test, ProcessKey);

public:
  enum DayPart {
    EVENING    = 'E', // Light level below threshhold, EVENING timer started, nightStart set.
    NIGHT      = 'N', // Light level below threshhold and EVENING timed out
    PREDAWN    = 'P', // Starts at (nightStart + lengthOfNight - predawnLength)
    MORNING    = 'M', // Light level above threshhold, MORNING timer started, dayStart set
    DAY        = 'D', // Light level above threshhold and MORNING timed out
  } ;
  
  void setup(const uint16_t initialValueMin, const uint16_t initialValueMax,
             const uint8_t nightDayThresholdPercentageValue);
  
  DayPart updateAverage(const uint16_t lightLevel);
  DayPart getDayPart();
  uint16_t getNightDayThreshold();
  
  uint16_t getPhotocellAvgValueMin()    {return photocellAvgValueMin;};
  uint16_t getPhotocellAvgValueMax()    {return photocellAvgValueMax;};
  uint16_t getPhotocellAvgValueCurrent(){return photocellAvgValueCurrent;};
  uint32_t getLengthOfNight()           {return lengthOfNight;};
  
  void setUpdateAverageTestMode(bool testModeFlag);
  
  // Primarily used to get access to private variables for testing
  
  
private:
  uint16_t photocellAvgValueCurrent;
  uint16_t photocellAvgValueMin;
  uint16_t photocellAvgValueMax;
  uint8_t  nightDayThresholdPercentage;
  uint32_t lengthOfNight;
  uint32_t nightStart;
  uint32_t dayStart;
  uint32_t update30secTimeout;
  uint32_t update5minTimeout;
  uint32_t eveningLength;
  uint32_t morningLength;
  uint32_t predawnLength;
  bool     updateAverageTestMode;
  
#define PHOTOCELLVALUESSIZE LUCKY7_TIME5MIN/LUCKY7_TIME30SEC
  uint16_t photocellValues[PHOTOCELLVALUESSIZE];
  uint8_t  photocellValuesIndex;
  
  void updatePhotocellAvgValues(uint16_t photocellAvgValue);
  void updateTimeOfDay();
  DayPart currentDayPart;
};

class UpDownMotor
{
private:
  FRIEND_TEST(UpDownMotorTest, Setup);
  FRIEND_TEST(UpDownMotorTest, GetInMotorUpDownMode);
  FRIEND_TEST(UpDownMotorTest, MotorUpStop);
  FRIEND_TEST(UpDownMotorTest, MotorDownStop);
  FRIEND_TEST(UpDownMotorTest, MotorUpStart);
  FRIEND_TEST(UpDownMotorTest, MotorDownStart);
  FRIEND_TEST(UpDownMotorTest, MotorUpUpdate);
  FRIEND_TEST(UpDownMotorTest, MotorDownUpdate);
  FRIEND_TEST(UpDownMotorTest, MotorUpdate);
  
  uint8_t * p_outputUp;
  uint8_t * p_outputDown;
  
  bool inMotorUpMode;
  bool inMotorDownMode;

  uint32_t motorUpStartTime;
  uint32_t motorDownStartTime;
  
  void motorUpUpdate();
  void motorDownUpdate();
  void motorUpStop();
  void motorDownStop();


public:
  void setup(uint8_t & oUp, uint8_t & p_oDown);

  void motorUpStart();
  void motorDownStart();

  uint8_t getMotorUpPower()   {return *p_outputUp;};
  uint8_t getMotorDownPower() {return *p_outputDown;};

  void motorUpdate();

  void motorStop() {motorUpStop(); motorDownStop();};

  bool getInMotorUpMode() {return inMotorUpMode;};
  bool getInMotorDownMode() {return inMotorDownMode;};
};

class  IRrecvMock;
class  decode_results;


class Lucky7
{
private:
  FRIEND_TEST(Lucky7Test, Setup);
  FRIEND_TEST(Lucky7Test, SaveOutputState);
  FRIEND_TEST(Lucky7Test, SetOutputStateFromSaved);
  FRIEND_TEST(Lucky7Test, Loop);
  FRIEND_TEST(Lucky7Test, Photocell1and2andBatteryVoltage);
  FRIEND_TEST(Lucky7Test, OutputMoveTo);
  FRIEND_TEST(B29Test, Statemap);
  FRIEND_TEST(Integration, CycleThroughDay);
  
  uint16_t photocell(uint16_t * pc);
  uint32_t irTimeout;
  uint16_t pc1[AVECNT], pc2[AVECNT], bc[AVECNT];
  uint8_t aveptr;
  void outputMoveTo(const uint8_t outputPin, uint8_t & currentValue,
                    uint8_t targetValue, const uint16_t stepDelay);

  uint8_t o1Saved,o2Saved,o3Saved,o4Saved,o5Saved,o6Saved,o7Saved;

public:

  enum BoardLightMode {
    LIGHT_ON,
    LIGHT_OFF,
    LIGHT_FAST_BLINK,
    LIGHT_SLOW_BLINK,
  };

  void boardLight(BoardLightMode mode, void (lightOn)(), void (lightOff)());

  uint8_t o1,o2,o3,o4,o5,o6,o7,o8,o13;


  void setup();

  uint32_t loop();
  uint32_t irLoop();

  void saveOutputState();
  void setOutputStateFromSaved();

  void o1On()             {o1 = ON;}
  void o1Set(uint8_t v)   {o1 = v;}
  void o1Off()            {o1 = 0;}
  void o1MoveTo(uint8_t v, const uint16_t stepdelay);
  void o1Toggle()         {o1 = o1?OFF:ON;};       
  
  void o2On()             {o2 = ON;}
  void o2Set(uint8_t v)   {o2 = v;}
  void o2Off()            {o2 = 0;}
  void o2MoveTo(uint8_t v, const uint16_t stepdelay);
  void o2Toggle()         {o2 = o2?OFF:ON;};       

  void o3On()             {o3 = ON;}
  void o3Set(uint8_t v)   {o3 = v;}
  void o3Off()            {o3 = 0;}
  void o3MoveTo(uint8_t v, const uint16_t stepdelay);
  void o3Toggle()         {o3 = o3?OFF:ON;};       

  void o4On()             {o4 = ON;}
  void o4Set(uint8_t v)   {o4 = v;}
  void o4Off()            {o4 = 0;}
  void o4MoveTo(uint8_t v, const uint16_t stepdelay);
  void o4Toggle()         {o4 = o4?OFF:ON;};       

  void o5On()             {o5 = ON;}
  void o5Set(uint8_t v)   {o5 = v;}
  void o5Off()            {o5 = 0;}
  void o5MoveTo(uint8_t v, const uint16_t stepdelay);
  void o5Toggle()         {o5 = o5?OFF:ON;};       

  void o6On()             {o6 = ON;}
  void o6Set(uint8_t v)   {o6 = v;}
  void o6Off()            {o6 = 0;}
  void o6MoveTo(uint8_t v, const uint16_t stepdelay);
  void o6Toggle()         {o6 = o6?OFF:ON;};       

  void o7On()             {o7 = ON;}
  void o7Set(uint8_t v)   {o7 = v;}
  void o7Off()            {o7 = 0;}
  void o7MoveTo(uint8_t v, const uint16_t stepdelay);
  void o7Toggle()         {o7 = o7?OFF:ON;};       

  void o8On()             {o8 = ON;}
  void o8Set(uint8_t v)   {o8 = v;}
  void o8Off()            {o8 = 0;}
  void o8MoveTo(uint8_t v, const uint16_t stepdelay);
  void o8Toggle()         {o8 = o8?OFF:ON;};       

  void o13On()            {o13 = ON;}
  void o13Set(uint8_t v)  {o13 = v;}
  void o13Off()           {o13 = 0;}
  void o13MoveTo(uint8_t v, const uint16_t stepdelay);
  void o13Toggle()        {o13 = o13?OFF:ON;};       

  uint16_t photocell1();
  uint16_t photocell2();

  float batteryVoltage();

};
#endif
