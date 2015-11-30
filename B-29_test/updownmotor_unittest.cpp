#include "updownmotor_unittest.h"

void UpDownMotorTest::SetUp() {
  uint8_t p_oUpArrayStack  [4] = {ON , ON, OFF, OFF};
  uint8_t p_oDownArrayStack[4] = {ON ,OFF,  ON, OFF};

  p_oUpArray   = p_oUpArrayStack  ;
  p_oDownArray = p_oDownArrayStack;
  p_oUp   = NULL;
  p_oDown = NULL;

  on  = ON ;
  off = OFF;
  p_on   = &on;
  p_off  = &off;
}

void UpDownMotorTest::TearDown() {
}

  

TEST_F(UpDownMotorTest, Setup) {
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t * p_oUp   = &p_oUpArray[i];
    uint8_t * p_oDown = &p_oDownArray[i];
    
    UpDownMotor udm = UpDownMotor();
    udm.setup(p_oUp, p_oDown);
    
    EXPECT_EQ(p_oUp  , udm.p_outputUp        );
    EXPECT_EQ(p_oDown, udm.p_outputDown      );
    EXPECT_EQ(false  , udm.inMotorUpMode     );
    EXPECT_EQ(false  , udm.inMotorDownMode   );
    EXPECT_EQ(0      , udm.motorUpStartTime  );
    EXPECT_EQ(0      , udm.motorDownStartTime);
  }
}

// TEST(UpDownMotorTest, MotorDownStop) {

//   uint8_t on  = ON ;
//   uint8_t off = OFF;
//   uint8_t * p_oUp   = &on;
//   uint8_t * p_oDown = &off;

//   UpDownMotor udm = UpDownMotor();
//   udm.setup(p_oUp, p_oDown);
