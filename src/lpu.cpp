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

void LPU::decode(uint8_t instr, uint64_t address) {
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

void LPU::nop0   (uint64_t address) { return; }
void LPU::nop1   (uint64_t address) { return; }
void LPU::jmp    (uint64_t address) {}
void LPU::cjmp   (uint64_t address) {}
void LPU::find   (uint64_t address) {}
void LPU::call   (uint64_t address) {}
void LPU::ret    (uint64_t address) {}
void LPU::add_a  (uint64_t address) {}
void LPU::add_b  (uint64_t address) {}
void LPU::sub_a  (uint64_t address) {}
void LPU::sub_b  (uint64_t address) {}
void LPU::sub_ab (uint64_t address) {}
void LPU::mov    (uint64_t address) {}
void LPU::movi   (uint64_t address) {}
void LPU::push_a (uint64_t address) {}
void LPU::push_b (uint64_t address) {}
void LPU::push_c (uint64_t address) {}
void LPU::pop_a  (uint64_t address) {}
void LPU::pop_b  (uint64_t address) {}
void LPU::pop_c  (uint64_t address) {}
void LPU::maloc  (uint64_t address) {}
void LPU::divide (uint64_t address) {}
