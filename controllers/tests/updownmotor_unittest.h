#ifndef UPDOWNMOTOR_UNITTEST_H
#define UPDOWNMOTOR_UNITTEST_H
#include "lucky7.h"
#include <gtest/gtest.h>

class UpDownMotorTest : public ::testing::Test {

protected:
  virtual void SetUp();
  //virtual void TearDown();

  uint8_t p_oUpArray  [4]; // Set in SetUp
  uint8_t p_oDownArray[4]; // Set in SetUp

  uint8_t * p_oUp  ;
  uint8_t * p_oDown;

  uint8_t on ;
  uint8_t off;
  uint8_t * p_on ;
  uint8_t * p_off;
};

#endif
