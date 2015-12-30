/**
 * Arduino IRremote mock
 */
#ifndef IRremote_h
#define IRremote_h

#include <stdint.h>
#include <gmock/gmock.h>

class decode_results {
public:
  int decode_type; // NEC, SONY, RC5, UNKNOWN
  unsigned int panasonicAddress; // This is only used for decoding Panasonic data
  unsigned long value; // Decoded value
  int bits; // Number of bits in decoded value
  volatile unsigned int *rawbuf; // Raw intervals in .5 us ticks
  int rawlen; // Number of records in rawbuf.
};

class IRrecvMock {
public:
  MOCK_METHOD1(decode, int (decode_results *));
  MOCK_METHOD0(enableIRIn, void ());
  MOCK_METHOD0(resume, void ());
};

class IRrecv_ {
  public:
  IRrecv_(int recvpin);

  int decode(decode_results *results);
  void enableIRIn();
  void resume();

  int recvPin;
};

typedef IRrecv_ IRrecv;

IRrecvMock* irrecvMockInstance();
void releaseIRrecvMock();

#endif // IRremote_h
