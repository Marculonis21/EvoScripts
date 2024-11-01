#ifndef LPU_HPP
#define LPU_HPP

#include "memorySpace.hpp"
#include "manager.hpp"
#include <cassert>
#include <cstdint>
#include <stack>
#include <string>

#include "lpu_addons.hpp"

class LPU {
  public:
	LPU(BaseMemoryType *memPtr, Manager *managerPtr, memorySpace memoryRecord);
	bool step();

	static std::string decode_tostring(uint8_t instr) {
		if(instr == 0) {
			return "EMPTY";
		}

		if (instrToStringMap.contains(instr)) {
			return instrToStringMap.at(instr);
		}

		return "UNKNOWN INSTRUCTION";
	}

  private:
	uint64_t regA;
	uint64_t regB;
	uint64_t regC;
	uint64_t ip;
	std::stack<uint64_t> stack;

	// just observers so I won't care about dangling pointers
	BaseMemoryType *memPtr;
	Manager *managerPtr;

	memorySpace memoryRecord;
	memorySpace memoryRecordOffspring;

	bool decode(uint8_t instr, uint64_t address);

	// instruction set
	bool nop0   (uint64_t address);
	bool nop1   (uint64_t address);
	bool jmp    (uint64_t address);
	bool ifz    (uint64_t address);
	bool ifnz   (uint64_t address);
	bool fndf   (uint64_t address);
	bool fndb   (uint64_t address);
	bool call   (uint64_t address);
	bool ret    (uint64_t address);
	bool zero_a (uint64_t address);
	bool add_a  (uint64_t address);
	bool add_b  (uint64_t address);
	bool add_c  (uint64_t address);
	bool sub_a  (uint64_t address);
	bool sub_b  (uint64_t address);
	bool sub_c  (uint64_t address);
	bool sub_ab (uint64_t address);
	/* bool mov    (uint64_t address); */
	bool movi   (uint64_t address);
	bool push_a (uint64_t address);
	bool push_b (uint64_t address);
	bool push_c (uint64_t address);
	bool pop_a  (uint64_t address);
	bool pop_b  (uint64_t address);
	bool pop_c  (uint64_t address);
	bool maloc  (uint64_t address);
	bool divide (uint64_t address);
};

#endif
