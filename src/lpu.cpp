#include "lpu.hpp"

LPU::LPU(const std::shared_ptr<BaseMemoryType> &memPtr, uint64_t ipStart) {
  this->memPtr = memPtr;
  ip = ipStart;
  regA, regB, regC = uint64_t();
}

bool LPU::step() {
  std::cout << "step ip: " << ip << std::endl;
  auto res = memPtr->fetch(ip);
  std::cout << "res " << std::to_string(res) << std::endl;

  return true;
}

void LPU::decode(uint8_t instr) {
  switch (instr) {
    case 0x01: return;
    case 0x02: return;
    case 0x03: return;
    case 0x04: return;
    case 0x05: return;
    case 0x06: return;
    case 0x07: return;
    case 0x08: return;
  }
}
