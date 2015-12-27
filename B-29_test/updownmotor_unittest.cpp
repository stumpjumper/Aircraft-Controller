#include "updownmotor_unittest.h"


void UpDownMotorTest::SetUp()
{
  p_oUp   = NULL;
  p_oDown = NULL;

  on  = ON ;
  off = OFF;
  p_on   = &on;
  p_off  = &off;
}

TEST_F(UpDownMotorTest, Setup) {
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t oUp   = p_oUpArray[i];
    uint8_t oDown = p_oDownArray[i];
    
    UpDownMotor udm = UpDownMotor();
    udm.setup(&oUp, &oDown);
    
    EXPECT_EQ(&oUp          , udm.p_outputUp        );
    EXPECT_EQ(&oDown        , udm.p_outputDown      );
    EXPECT_EQ(p_oUpArray[i]  , *udm.p_outputUp       ) << "oUp = " << int(oUp) << ",p_oUpArray[" << int(i) << "] = " << int(p_oUpArray[i]);
    EXPECT_EQ(p_oDownArray[i], *udm.p_outputDown     ) << "oDown = " << int(oDown) << ", p_oDownArray[" << int(i) << "] = " << int(p_oDownArray[i]);
    EXPECT_EQ(false          , udm.inMotorUpMode     );
    EXPECT_EQ(false          , udm.inMotorDownMode   );
    EXPECT_EQ(0              , udm.motorUpStartTime  );
    EXPECT_EQ(0              , udm.motorDownStartTime);
  }
}

TEST_F(UpDownMotorTest, MotorUpStop) {
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t oUp   = p_oUpArray[i];
    uint8_t oDown = p_oDownArray[i];
    
    UpDownMotor udm = UpDownMotor();
    udm.setup(&oUp, &oDown);

    udm.motorUpStop();
    EXPECT_EQ(OFF  , *udm.p_outputUp);
    EXPECT_EQ(false, udm.inMotorUpMode );
    EXPECT_EQ(false, udm.inMotorDownMode );
  }
}

TEST_F(UpDownMotorTest, MotorDownStop) {
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t oUp   = p_oUpArray[i];
    uint8_t oDown = p_oDownArray[i];
    
    UpDownMotor udm = UpDownMotor();
    udm.setup(&oUp, &oDown);

    udm.motorDownStop();
    EXPECT_EQ(OFF  , *udm.p_outputDown);
    EXPECT_EQ(false, udm.inMotorUpMode );
    EXPECT_EQ(false, udm.inMotorDownMode );
  }
}

TEST_F(UpDownMotorTest, MotorUpStart) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillOnce(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  uint32_t millisSet = 9283*60*60;
  arduinoMock->setMillisRaw(millisSet);

  bool inMotorUpModeArray[] = {false, true};

  UpDownMotor udm = UpDownMotor();
  uint8_t oUp   = ON;
  uint8_t oDown = OFF;
  udm.setup(&oUp, &oDown);
  
  for (uint8_t i = 0; i < 2; i++) {
    udm.inMotorUpMode = inMotorUpModeArray[i];

    udm.motorUpStart();
    EXPECT_EQ(millisSet, udm.motorUpStartTime);
    EXPECT_EQ(true, udm.inMotorUpMode);
  }

  releaseArduinoMock();
}

TEST_F(UpDownMotorTest, MotorDownStart) {
  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillOnce(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));

  uint32_t millisSet = 9283*60*60;
  arduinoMock->setMillisRaw(millisSet);

  bool inMotorDownModeArray[] = {false, true};

  uint8_t oUp   = ON;
  uint8_t oDown = OFF;
  UpDownMotor udm = UpDownMotor();
  udm.setup(&oUp, &oDown);
  
  for (uint8_t i = 0; i < 2; i++) {
    udm.inMotorDownMode = inMotorDownModeArray[i];

    udm.motorDownStart();
    EXPECT_EQ(millisSet, udm.motorDownStartTime);
    EXPECT_EQ(true, udm.inMotorDownMode);
  }

  releaseArduinoMock();
}

TEST_F(UpDownMotorTest, MotorUpUpdate) {

  // States:
  // 1) Not in motor up  mode
  //    p_outputUp = OFF
  // 2) In motor up mode
  //    a) Before start deley
  //       inMotorUpMode = true
  //       p_outputUp = OFF
  //    b) Before up/down timeout
  //       inMotorUpMode = true
  //       p_outputUp = ON
  //    c) After up/down timeout
  //       inMotorUpMode = false
  //       p_outputUp = OFF

  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillRepeatedly(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));


  uint32_t startTime = 9283*60*60;
  uint8_t oUp   = OFF;
  uint8_t oDown = OFF;
  UpDownMotor udm = UpDownMotor();
  udm.setup(&oUp, &oDown);

  udm.inMotorUpMode = false;
  udm.motorUpStartTime = startTime;
  
  // State (1)
  udm.motorUpUpdate();
  EXPECT_EQ(oUp, *udm.p_outputUp);
  EXPECT_EQ(OFF, oUp);
  EXPECT_EQ(false, udm.inMotorUpMode);

  // State (2a)
  udm.inMotorUpMode = true;
  arduinoMock->setMillisRaw(startTime + LUCKY7_TIMEMOTORDELAY/2);
  udm.motorUpUpdate();
  EXPECT_EQ(oUp, *udm.p_outputUp);
  EXPECT_EQ(OFF, oUp);
  EXPECT_EQ(true, udm.inMotorUpMode);

  // State (2b)
  arduinoMock->setMillisRaw(startTime + LUCKY7_TIMEMOTORDELAY + 1);
  udm.motorUpUpdate();
  EXPECT_EQ(oUp, *udm.p_outputUp);
  EXPECT_EQ(ON, oUp);
  EXPECT_EQ(true, udm.inMotorUpMode);

  // State (2c)
  arduinoMock->setMillisRaw(startTime + LUCKY7_TIMEOUTMOTORUPDOWN + 1);
  udm.motorUpUpdate();
  EXPECT_EQ(oUp, *udm.p_outputUp);
  EXPECT_EQ(OFF, oUp);
  EXPECT_EQ(false, udm.inMotorUpMode);

  releaseArduinoMock();
}

TEST_F(UpDownMotorTest, MotorDownUpdate) {

  // States:
  // See MotorUpUpdate test

  ArduinoMock * arduinoMock = arduinoMockInstance();

  EXPECT_CALL(*arduinoMock, millis())
    .WillRepeatedly(testing::InvokeWithoutArgs(
                arduinoMock, &ArduinoMock::getMillis));


  uint32_t startTime = 9283*60*60;
  uint8_t oUp   = OFF;
  uint8_t oDown = OFF;
  UpDownMotor udm = UpDownMotor();
  udm.setup(&oUp, &oDown);

  udm.inMotorDownMode = false;
  udm.motorDownStartTime = startTime;
  
  // State (1)
  udm.motorDownUpdate();
  EXPECT_EQ(oDown, *udm.p_outputDown);
  EXPECT_EQ(OFF, oDown);
  EXPECT_EQ(false, udm.inMotorDownMode);

  // State (2a)
  udm.inMotorDownMode = true;
  arduinoMock->setMillisRaw(startTime + LUCKY7_TIMEMOTORDELAY/2);
  udm.motorDownUpdate();
  EXPECT_EQ(oDown, *udm.p_outputDown);
  EXPECT_EQ(OFF, oDown);
  EXPECT_EQ(true, udm.inMotorDownMode);

  // State (2b)
  arduinoMock->setMillisRaw(startTime + LUCKY7_TIMEMOTORDELAY + 1);
  udm.motorDownUpdate();
  EXPECT_EQ(oDown, *udm.p_outputDown);
  EXPECT_EQ(ON, oDown);
  EXPECT_EQ(true, udm.inMotorDownMode);

  // State (2c)
  arduinoMock->setMillisRaw(startTime + LUCKY7_TIMEOUTMOTORUPDOWN + 1);
  udm.motorDownUpdate();
  EXPECT_EQ(oDown, *udm.p_outputDown);
  EXPECT_EQ(OFF, oDown);
  EXPECT_EQ(false, udm.inMotorDownMode);

  releaseArduinoMock();
}



// * void UpDownMotor::setup(uint8_t * p_oUp, uint8_t * p_oDown)
// * void motorUpStop()
// * void motorDownStop()
// * void motorUpStart()
// * void motorDownStart()
// * void motorUpUpdate()
// * void motorDownUpdate()
// void motorUpdate()



