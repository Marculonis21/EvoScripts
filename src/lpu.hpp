#ifndef LPU_HPP
#define LPU_HPP

#include "memorySpace.hpp"
#include "manager.hpp"
#include <cstdint>
#include <stack>
#include <string>

class LPU {
  public:
	LPU(BaseMemoryType *memPtr, Manager *managerPtr, memorySpace memoryRecord);
	bool step();

	static std::string decode_tostring(uint8_t instr) {
		switch (instr) {
		case 0x00:
			return "----";
		case 0x01:
			return "nop0";
		case 0x02:
			return "nop1";
		case 0x03:
			return "jmp";
		case 0x04:
			return "cjmp";
		case 0x05:
			return "find";
		case 0x06:
			return "call";
		case 0x07:
			return "ret";
		case 0x08:
			return "zero_a";
		case 0x09:
			return "add_a";
		case 0x0a:
			return "add_b";
		case 0x0b:
			return "sub_a";
		case 0x0c:
			return "sub_b";
		case 0x0d:
			return "sub_ab";
		case 0x0e:
			return "mov";
		case 0x0f:
			return "movi";
		case 0x10:
			return "push_a";
		case 0x11:
			return "push_b";
		case 0x12:
			return "push_c";
		case 0x13:
			return "pop_a";
		case 0x14:
			return "pop_b";
		case 0x15:
			return "pop_c";
		case 0x16:
			return "maloc";
		case 0x17:
			return "divide";
		default:
			return "UNKNOWN";
		}
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
	bool cjmp   (uint64_t address);
	bool find   (uint64_t address);
	bool call   (uint64_t address);
	bool ret    (uint64_t address);
	bool zero_a (uint64_t address);
	bool add_a  (uint64_t address);
	bool add_b  (uint64_t address);
	bool sub_a  (uint64_t address);
	bool sub_b  (uint64_t address);
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
