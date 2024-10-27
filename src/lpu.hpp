#ifndef LPU_HPP
#define LPU_HPP

#include "memorySpace.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <stack>
#include <string>

class LPU {
public:
  LPU(const std::shared_ptr<BaseMemoryType> &memPtr, uint64_t ipStart);
  bool step();

private:
  uint64_t regA;
  uint64_t regB;
  uint64_t regC;
  uint64_t ip;
  std::stack<uint64_t> stack;

  std::shared_ptr<BaseMemoryType> memPtr;

  void decode(uint8_t instr, uint64_t address);

  // instruction set
  void nop0   (uint64_t address);
  void nop1   (uint64_t address);
  void jmp    (uint64_t address);
  void cjmp   (uint64_t address);
  void find   (uint64_t address);
  void call   (uint64_t address);
  void ret    (uint64_t address);
  void add_a  (uint64_t address);
  void add_b  (uint64_t address);
  void sub_a  (uint64_t address);
  void sub_b  (uint64_t address);
  void sub_ab (uint64_t address);
  void mov    (uint64_t address);
  void movi   (uint64_t address);
  void push_a (uint64_t address);
  void push_b (uint64_t address);
  void push_c (uint64_t address);
  void pop_a  (uint64_t address);
  void pop_b  (uint64_t address);
  void pop_c  (uint64_t address);
  void maloc  (uint64_t address);
  void divide (uint64_t address);
};

#endif
