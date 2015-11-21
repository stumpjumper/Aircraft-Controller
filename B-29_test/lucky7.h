#ifndef LUCKY7_H
#define LUCKY7_H
#include "Arduino.h"
#include "Serial.h"
#include "gtest/gtest_prod.h"

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
#define LUCKY7_TIME12HOUR         43200000U // 24 hours

class Lucky7;

class Light
{

/*
  decayFlash
  see F16 Light Dimming Plot.ods
  pwm = a+b*exp(d*t/e)
*/

private:

public:

  void decayFlashInit();
  void decayFlash();
  void decayFlashOff();

  void flashInit();
  void flash();
  void flashOff();

  void steadyOnInit();
  void steadyOn();
  void steadyOnOff();

};

class TimeOfDay
{
public:

  FRIEND_TEST(TimeOfDayTest, UpdateValuesAndTimeOfDay);

 enum DayPart {
   EVENING    = 'E', // Light level below threshhold, EVENING timer started, nightStart set.
   NIGHT      = 'N', // Light level below threshhold and EVENING timed out
   PREDAWN    = 'P', // Starts at (nightStart + lenghtOfNight - predawnLength)
   MORNING    = 'M', // Light level above threshhold, MORNING timer started, dayStart set
   DAY        = 'D', // Light level above threshhold and MORNING timed out
  } ;
  
  TimeOfDay(uint16_t initialValueMin, uint16_t initialValueMax,
            uint8_t nightDayThresholdPercentageValue);

  DayPart updateAverage(const uint16_t lightLevel);
  DayPart getDayPart();
  uint16_t getNightDayThreshold();

  // Primarily used to get access to private variables for testing
  uint32_t getPhotocellAvgValueCurrent() {return photocellAvgValueCurrent;};
  uint32_t getPhotocellAvgValueMin() {return photocellAvgValueMin;};
  uint32_t getPhotocellAvgValueMax() {return photocellAvgValueMax;};
  uint8_t  getNightDayThresholdPercentage() {return nightDayThresholdPercentage;};
  uint32_t getLenghtOfNight() {return lenghtOfNight;};
  uint32_t getNightStart() {return nightStart;};
  uint32_t getDayStart() {return dayStart;};
  uint32_t getUpdate30secTimeout() {return update30secTimeout;};
  uint32_t getUpdate5minTimeout() {return update5minTimeout;};
  uint32_t getEveningLength() {return eveningLength;};
  uint32_t getMorningLength() {return morningLength;};
  uint32_t getPredawnLength() {return predawnLength;};
  uint8_t  getPhotocellValuesIndex() {return photocellValuesIndex;};

  void setPhotocellAvgValueCurrent(uint32_t val) {photocellAvgValueCurrent = val;};
  void setPhotocellAvgValueMin(uint32_t val) {photocellAvgValueMin = val;};
  void setPhotocellAvgValueMax(uint32_t val) {photocellAvgValueMax = val;};
  void setNightDayThresholdPercentage(uint8_t val) {nightDayThresholdPercentage = val;};
  void setLenghtOfNight(uint32_t val) {lenghtOfNight = val;};
  void setNightStart(uint32_t val) {nightStart = val;};
  void setDayStart(uint32_t val) {dayStart = val;};
  void setUpdate30secTimeout(uint32_t val) {update30secTimeout = val;};
  void setUpdate5minTimeout(uint32_t val) {update5minTimeout = val;};
  void setEveningLength(uint32_t val) {eveningLength = val;};
  void setMorningLength(uint32_t val) {morningLength = val;};
  void setPredawnLength(uint32_t val) {predawnLength = val;};
  void setPhotocellValuesIndex(uint8_t val) {photocellValuesIndex = val;};
  void setCurrentDayPart(DayPart val) {currentDayPart = val;};


private:
  uint32_t photocellAvgValueCurrent;
  uint32_t photocellAvgValueMin;
  uint32_t photocellAvgValueMax;
  uint8_t  nightDayThresholdPercentage;
  uint32_t lenghtOfNight;
  uint32_t nightStart;
  uint32_t dayStart;
  uint32_t update30secTimeout;
  uint32_t update5minTimeout;
  uint32_t eveningLength;
  uint32_t morningLength;
  uint32_t predawnLength;
  

#define PHOTOCELLVALUESSIZE LUCKY7_TIME5MIN/LUCKY7_TIME30SEC
  uint16_t photocellValues[PHOTOCELLVALUESSIZE];
  uint8_t  photocellValuesIndex;

  TimeOfDay();
  void updateValuesAndTimeOfDay(uint16_t photocellAvgValue);
  DayPart currentDayPart;
  
};

class UpDownMotor
{
private:

  uint8_t & outputUp;
  uint8_t & outputDown;

  bool inMotorUpMode;
  bool inMotorDownMode;

  uint32_t motorUpStartTime;
  uint32_t motorDownStartTime;
  
  void motorUpUpdate();
  void motorDownUpdate();
  void motorUpStop();
  void motorDownStop();


public:
  UpDownMotor();
  UpDownMotor(uint8_t & oUp, uint8_t & oDown);

  void motorUpStart();
  void motorDownStart();

  uint8_t getMotorUpPower()   {return outputUp;}
  uint8_t getMotorDownPower() {return outputDown;}

  void motorUpdate();

  void motorStop() {motorUpStop();motorDownStop();}
};

class Lucky7
{
private:
  uint32_t irTimeout;
  uint16_t pc1[AVECNT], pc2[AVECNT], bc[AVECNT];
  uint8_t aveptr;
  uint8_t outputMoveTo(const uint8_t outputStart, uint8_t v, const uint16_t stepdelay);

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

//void o3On()             {o3 = ON;}
//void o3Set(uint8_t v)   {o3 = v;}
//void o3Off()            {o3 = 0;}
//void o3MoveTo(uint8_t v, const uint16_t stepdelay);
//void o3Toggle()         {o3 = o3?OFF:ON;};       

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

//void o7On()             {o7 = ON;}
//void o7Set(uint8_t v)   {o7 = v;}
//void o7Off()            {o7 = 0;}
//void o7MoveTo(uint8_t v, const uint16_t stepdelay);
//void o7Toggle()         {o7 = o7?OFF:ON;};       

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
