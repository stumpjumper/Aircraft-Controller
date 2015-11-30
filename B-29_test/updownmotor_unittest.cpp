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


