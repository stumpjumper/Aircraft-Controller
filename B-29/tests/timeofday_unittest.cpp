#include "lucky7.h"
#include <gtest/gtest.h>

TEST(TimeOfDayTest, setup) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(1);

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(0,1000,10);

  // Test TimeOfDay.setup()
  EXPECT_EQ(10, tod.nightDayThresholdPercentage);
  EXPECT_EQ(0, tod.photocellAvgValueMin);
  EXPECT_EQ(1000, tod.getPhotocellAvgValueMax());
  EXPECT_EQ(0, tod.getPhotocellAvgValueCurrent());

  EXPECT_EQ(LUCKY7_TIME30SEC, tod.update30secTimeout);
  EXPECT_EQ(LUCKY7_TIME5MIN, tod.update5minTimeout);
  EXPECT_EQ(LUCKY7_TIME4HOUR, tod.eveningLength);
  EXPECT_EQ(LUCKY7_TIME2HOUR, tod.morningLength);
  EXPECT_EQ(0, tod.photocellValuesIndex);

  EXPECT_EQ(LUCKY7_TIME12HOUR, tod.lengthOfNight);
  EXPECT_EQ(0, tod.nightStart);
  EXPECT_EQ(0, tod.dayStart);
  EXPECT_EQ(TimeOfDay::DAY, tod.getDayPart());

  releaseArduinoMock();
}

TEST(TimeOfDayTest, getNightDayThreshold) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(1);

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(0,1000,10);

  tod.photocellAvgValueMin = 0;
  tod.photocellAvgValueMax = 1000;
  tod.nightDayThresholdPercentage = 22;
  EXPECT_EQ(220, tod.getNightDayThreshold());

  tod.photocellAvgValueMin = 11;
  tod.photocellAvgValueMax = 1010;
  tod.nightDayThresholdPercentage = 22;
  EXPECT_EQ(230, tod.getNightDayThreshold());

  tod.photocellAvgValueMin = 10;
  tod.photocellAvgValueMax = 1015;
  tod.nightDayThresholdPercentage = 22;
  EXPECT_EQ(231, tod.getNightDayThreshold());

  releaseArduinoMock();
}

TEST(TimeOfDayTest, UpdatePhotocellAvgValues) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(1);

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(500,501,10);

  uint16_t values   [10] = {801, 900, 950,1000, 410, 323, 281, 102,  13, 824};
  uint16_t valuesMin[10] = {500, 500, 500, 500, 410, 323, 281, 102,  13,  13};
  uint16_t valuesMax[10] = {801, 900, 950,1000,1000,1000,1000,1000,1000,1000};
    

  for (uint8_t i = 0; i < 10; i++) {
    tod.updatePhotocellAvgValues(values[i]);
    EXPECT_EQ(values   [i], tod.getPhotocellAvgValueCurrent());
    EXPECT_EQ(valuesMin[i], tod.getPhotocellAvgValueMin    ());
    EXPECT_EQ(valuesMax[i], tod.getPhotocellAvgValueMax    ());
  }

  releaseArduinoMock();
}

TEST(TimeOfDayTest, UpdateAverage) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .Times(testing::AtLeast(1));

  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(100,250,10);

  uint16_t * lightLevel;
  uint16_t   lightLevelArray[4][10] =
    {{ 10, 100,  30,  70,  50,  80,  40,  60,  90,  20},   // Avg = 55 (Note: Drop high and low)
     {  5,  95,  25,  65,  45,  75,  35,  55,  85,  15},   // Avg = 50
     { 50, 950, 250, 650, 450, 750, 350, 550, 850, 150},   // Avg = 500
     {100,1000, 300, 700, 500, 800, 400, 600, 900, 200} }; // Avg = 550
  uint32_t   avgValue   [4] = { 55,  50, 500, 550};
  uint32_t   avgValueMin[4] = { 55,  50,  50,  50};
  uint32_t   avgValueMax[4] = {250, 250, 500, 550};
  uint16_t   lightLevelIgnore = 10000;
  uint32_t   millisIncr = 15;

  arduinoMock->setMillisRaw(10);

  for (uint8_t j = 0; j < 4; j++) {
    lightLevel = lightLevelArray[j];
    for (uint8_t i = 0; i < 10; i++) {
      arduinoMock->addMillisSecs(millisIncr);
      tod.updateAverage(lightLevelIgnore);
      
      arduinoMock->addMillisSecs(millisIncr);
      tod.updateAverage(lightLevel[i]);
    }
    EXPECT_EQ(avgValue   [j], tod.getPhotocellAvgValueCurrent());
    EXPECT_EQ(avgValueMin[j], tod.getPhotocellAvgValueMin());
    EXPECT_EQ(avgValueMax[j], tod.getPhotocellAvgValueMax());
  }

  releaseArduinoMock();
}

TEST(TimeOfDayTest, UpdateTimeOfDay) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  const TimeOfDay::DayPart day     = TimeOfDay::DAY    ;
  const TimeOfDay::DayPart evening = TimeOfDay::EVENING;
  const TimeOfDay::DayPart night   = TimeOfDay::NIGHT  ;
  const TimeOfDay::DayPart predawn = TimeOfDay::PREDAWN;
  const TimeOfDay::DayPart morning = TimeOfDay::MORNING;

  const uint32_t hr12     = LUCKY7_TIME12HOUR;
  const uint32_t min61    =    61*60*1000;
  const uint32_t min482   =   482*60*1000;
  const uint32_t min543   =   543*60*1000;
  const uint32_t min1505  =  1505*60*1000;
  const uint32_t min1987  =  1987*60*1000;
  const uint32_t min2949  =  2949*60*1000;
  const uint32_t min3431  =  3431*60*1000;


  EXPECT_CALL(*arduinoMock, millis())
    .Times(testing::AtLeast(1));


  // Coming out of setup:
  // tod.eveningLength = LUCKY7_TIME4HOUR;
  // tod.predawnLength = LUCKY7_TIME2HOUR;
  // tod.morningLength = LUCKY7_TIME2HOUR;
  // tod.lengthOfNight = LUCKY7_TIME12HOUR;

  // D=68, E=69, M=77, N=78, P=80

  const uint8_t numData = 62;

  uint16_t addMins[numData] =
    {60,                // day      60
     1,60,60,60,60,     // evening  61,121,181,241,301
     1,60,60,60,60,     // night    302,362,422,482,542
     1,60,60,           // morning  543,603,663
     1,60,60,60,60,540, // day      664,724,784,844,904,1444
     // Day 2
     60,                // day      1504
     1,60,60,60,60,     // evening  1505,1565,1625,1685,1745,
     1,60,60,           // night    1746,1806,1866,
     2,58,60,           // predawn  1868,1926,1986,
     1,60,60,           // morning  1987,2047,2107,
     1,60,60,60,60,540, // day      2108,2168,2228,2288,2348,2888
     // Day 3
     60,                // day      2948,
     1,60,60,60,60,     // evening  2949,3009,3069,3129,3189,
     1,60,60,           // night    3190,3250,3310,
     2,58,60,           // predawn  3312,3370,3430,
     1,60,60,           // morning  3431,3491,3551,
     1,60,60,60,60,540  // day      3552,3612,3672,3732,3792,4332
    };  
  uint16_t photocellValueArray[numData] =
    {850,                         //  0
     150, 150, 150, 150, 150,     //  1- 5
     150, 150, 150, 150, 150,     //  6-10
     850, 850, 850,               // 11-13
     850, 850, 850, 850, 850, 850,// 14-19
     // Day 2
     850,                         // 20
     150, 150, 150, 150, 150,     // 21-25
     150, 150, 150,               // 26-28
     150, 150, 150,               // 29-31
     850, 850, 850,               // 32-34
     850, 850, 850, 850, 850, 850,// 35-40
     // Day 3
     850,                         // 41
     150, 150, 150, 150, 150,     // 42-56
     150, 150, 150,               // 47-49
     150, 150, 150,               // 50-52
     850, 850, 850,               // 53-55
     850, 850, 850, 850, 850, 850 // 56-61
    };
  TimeOfDay::DayPart dayPartArray[numData] =
    {day,
     evening, evening, evening, evening, evening,
     night, night, night, night, night,
     morning, morning, morning,
     day, day, day, day, day, day,
     // Day 2
     day,
     evening, evening, evening, evening, evening,
     night, night, night,
     predawn, predawn, predawn,
     morning, morning, morning,
     day, day, day, day, day, day,
     // Day 3
     day,
     evening, evening, evening, evening, evening,
     night, night, night,
     predawn, predawn, predawn,
     morning, morning, morning,
     day, day, day, day, day, day,
    };
  uint32_t nightStartArray[numData] =
    {0,
     min61, min61, min61, min61, min61,
     min61, min61, min61, min61, min61,
     min61, min61, min61,
     min61, min61, min61, min61, min61, min61,
     // Day 2
     min61  ,
     min1505, min1505, min1505, min1505, min1505,
     min1505, min1505, min1505,
     min1505, min1505, min1505,
     min1505, min1505, min1505,
     min1505, min1505, min1505, min1505, min1505, min1505,
     // Day 3
     min1505,
     min2949, min2949, min2949, min2949, min2949,
     min2949, min2949, min2949,
     min2949, min2949, min2949,
     min2949, min2949, min2949,
     min2949, min2949, min2949, min2949, min2949, min2949,
    };
  uint32_t dayStartArray[numData] =
    {0,                                                    //  0   
     0, 0, 0, 0, 0,                                        //  1- 5
     0, 0, 0, 0, 0,                                        //  6-10
     min543, min543, min543,                               // 11-13
     min543, min543, min543, min543, min543, min543,       // 14-19
     // Day 2                                                         
     min543 ,                                              // 20   
     min543 , min543 , min543 , min543 , min543 ,          // 21-25
     min543 , min543 , min543 ,                            // 26-28
     min543 , min543 , min543 ,                            // 29-31
     min1987, min1987, min1987,                            // 32-34
     min1987, min1987, min1987, min1987, min1987, min1987, // 35-40
     // Day 3                                                         
     min1987,                                              // 41   
     min1987, min1987, min1987, min1987, min1987,          // 42-56
     min1987, min1987, min1987,                            // 47-49
     min1987, min1987, min1987,                            // 50-52
     min3431, min3431, min3431,                            // 53-55
     min3431, min3431, min3431, min3431, min3431, min3431, // 56-61
    };
  uint32_t lengthOfNightArray[numData] =
    {hr12  ,
     hr12  , hr12  , hr12  , hr12  , hr12,
     hr12  , hr12  , hr12  , hr12  , hr12,
     min482, min482, min482, 
     min482, min482, min482, min482, min482, min482,
     // Day 2
     min482,
     min482, min482, min482, min482, min482, 
     min482, min482, min482,
     min482, min482, min482,
     min482, min482, min482, 
     min482, min482, min482, min482, min482, min482,
     // Day 3
     min482,
     min482, min482, min482, min482, min482, 
     min482, min482, min482,
     min482, min482, min482,
     min482, min482, min482, 
     min482, min482, min482, min482, min482, min482,
    };
  
  arduinoMock->setMillisRaw(0);

  TimeOfDay tod = TimeOfDay();
  tod.setup(100,900,10);

  EXPECT_EQ(TimeOfDay::DAY, tod.getDayPart());

  for (uint8_t i = 0; i < numData; i++) {
    arduinoMock->addMillisMins(addMins[i]);
    tod.updatePhotocellAvgValues(photocellValueArray[i]);
    tod.updateTimeOfDay();
    EXPECT_EQ(char(dayPartArray[i]), char(tod.getDayPart()))  << "i = " << int(i);
    EXPECT_EQ(nightStartArray   [i], tod.nightStart)    << "i = " << int(i);
    EXPECT_EQ(dayStartArray     [i], tod.dayStart)      << "i = " << int(i);
    EXPECT_EQ(lengthOfNightArray[i], tod.lengthOfNight) << "i = " << int(i);
  }

  releaseArduinoMock();
}




