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

  UpDownMotor udm = UpDownMotor();
  uint8_t oUp   = ON;
  uint8_t oDown = OFF;
  udm.setup(&oUp, &oDown);
  
  for (uint8_t i = 0; i < 2; i++) {
    udm.inMotorDownMode = inMotorDownModeArray[i];

    udm.motorDownStart();
    EXPECT_EQ(millisSet, udm.motorDownStartTime);
    EXPECT_EQ(true, udm.inMotorDownMode);
  }

  releaseArduinoMock();
}


// * void UpDownMotor::setup(uint8_t * p_oUp, uint8_t * p_oDown)
// * void motorUpStop()
// * void motorDownStop()
// * void motorUpStart()
// * void motorDownStart()
// void motorUpdate()
// void motorUpUpdate()
// void motorDownUpdate()



