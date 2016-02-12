/**
 * IRremoteMock header
 */
#ifndef IRREMOTEMOCK_H
#define IRREMOTEMOCK_H

#include <stdint.h>
#include <gmock/gmock.h>

#include "IRremote.h"

class IRrecvMock : public IRrecv  {
  
public:
  IRrecvMock(int recvpin);

  MOCK_METHOD1(decode, int (decode_results *));
  MOCK_METHOD0(enableIRIn, void ());
  MOCK_METHOD0(resume, void ());
};

class IRrecvMockWrapper {

private:
  IRrecvMock * irrecvMock;

public:
  IRrecvMock & getIRrecvMock();
}

IRrecvMockWrapper* irrecvMockWrapperInstance();
void releaseIRrecvMockWrapper();

#endif // IRREMOTEMOCK_H
