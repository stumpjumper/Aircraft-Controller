#include "IRremoteMock.h"

// static IRrecvMock* irrecvMock = NULL;
// IRrecvMock* irrecvMockInstance() {
//   if(!irrecvMock) {
//     irrecvMock = new IRrecvMock();
//   }
//   return irrecvMock;
// }

// void releaseIRrecvMock() {
//   if(irrecvMock) {
//     delete IRrecvMock;
//     irrecvMock = NULL;
//   }
// }

IRrecvMock::IRrecvMock(int recvpin): IRrecv(recvpin)
{
}

static int myRecvpin;
IRrecv::IRrecv(int recvpin)
{
  myRecvpin = recvpin;
}

IRrecv::~IRrecv()
{
}

static decode_results * myDecodeResults;
int IRrecv::decode(decode_results *results)
{
  myDecodeResults = results;
  std::cerr << "Hello from IRrecv::decode" << std::endl;
  return 1;
}

void IRrecv::enableIRIn()
{
  std::cerr << "Hello from IRrecv::enableIRIn" << std::endl;
}
void IRrecv::resume() 
{
  std::cerr << "Hello from IRrecv::resume()" << std::endl;
}


