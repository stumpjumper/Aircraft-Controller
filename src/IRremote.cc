#include "arduino-mock/IRremote.h"

static IRrecvMock* gIRrecvMock = NULL;
IRrecvMock* irrecvMockInstance() {
  if(!gIRrecvMock) {
    gIRrecvMock = new IRrecvMock();
  }
  return gIRrecvMock;
}

void releaseIRrecvMock() {
  if(gIRrecvMock) {
    delete gIRrecvMock;
    gIRrecvMock = NULL;
  }
}

IRrecv_::IRrecv_(int recvpin) {
  recvPin = recvpin;
}


int IRrecv_::decode(decode_results *results) {
  return gIRrecvMock->decode(results);
}

void IRrecv_::enableIRIn() {
  gIRrecvMock->enableIRIn();
}

void IRrecv_::resume() {
  gIRrecvMock->resume();
}

//// Preinstantiate Objects
//IRrecv_ IRrecv;
