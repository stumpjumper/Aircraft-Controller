#ifndef INTEGRATION_TEST_H
#define INTEGRATION_TEST_H
#include <gtest/gtest.h>

class Integration: public ::testing::Test {

private:
  uint16_t pc1Value;
  uint16_t pc2Value;
  uint16_t bcValue ;

public:
  void setAnalogReadValue(int pin, uint16_t pinValue);
  uint16_t getAnalogReadValue(int pin);

protected:
  virtual void SetUp();
  //virtual void TearDown();
  
  // static void SetUpTestCase()
  //{
  //};
  
  //static void TearDownTestCase()
  //{
  //}

};

#endif // INTEGRATION_TEST_H

    

