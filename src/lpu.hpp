#pragma once

#include "memory.hpp"
#include <cassert>
#include <cstdint>
#include <stack>
#include <string>
#include <iostream>

#include "lpu_addons.hpp"

// ugly!
class Manager;
class EvoDex;

struct LPUObservers {
	BaseMemoryType *memory;
	Manager *manager;
	Randomizer *randomizer;
	EvoDex *evoDex;
};

class LPU {
  public:
	LPU(LPUHandle handle, LPUHandle parent, const LPUObservers &observers, MemorySpace memoryRecord, uint64_t dateofbirth);
	bool step();
	operator std::string() const;
	bool operator==(const LPU &other) const;

	static std::string decode_tostring(uint8_t instr) {
		if (instr == 0) {
			return "EMPTY";
		}

		if (instrToStringMap.contains(instr)) {
			return instrToStringMap.at(instr);
		}

		return "UNKNOWN INSTRUCTION";
	}

	uint64_t currentIP() const { return ip; }
	uint64_t errorCount() const { return errors; }
	LPUHandle getHandle() const { return handle; }
	std::pair<const MemorySpace, const MemorySpace> getMemRecords() const { return std::make_pair(memoryRecord, memoryRecordOffspring); }

	struct Hash {
		static uint64_t build(const LPU &lpu);

		uint64_t operator()(const LPU &lpu) const noexcept {
			return lpu.memHash;
		}

		static constexpr uint64_t p = 31;
		static constexpr uint64_t m = 1e9 + 9;

		// lambda constexpr
		static constexpr uint64_t N = 10000;
		static constexpr std::array<uint64_t, N> ppow = [](){
			std::array<uint64_t, N> ppow{1};
			for (uint64_t i = 1; i < N; ++i) {
				ppow[i] = (ppow[i - 1] * p) % m;
			}
			return ppow;
		}();
	};

	struct Instructions {
		std::vector<std::uint8_t> vec;

		Instructions() = default;
		Instructions(const LPU &lpu);

		bool operator==(const Instructions &other) const {
			if (vec.size() != other.vec.size()) { return false; }

			for (int i = 0; i < vec.size(); ++i) {
				if (vec[i] != other.vec[i]) { return false; }
			}

			return true;
		}
	};

	struct Metadata {
		LPUHandle handle;
		LPUHandle parent;
		/* LPUHandle DNApre; */
		/* LPUHandle parentDNApre; */

		std::vector<LPUHandle> DNApre;

		uint64_t dateofbirth;
		Instructions instructions;
		uint64_t occurence=0;

		Metadata() = default;
		Metadata(const LPU &lpu);

		bool operator==(const Metadata &other) const {
			return this->instructions == other.instructions;
		}
	};

  private:
	LPUHandle handle;
	LPUHandle parent;

	uint64_t regA;
	uint64_t regB;
	uint64_t regC;
	uint64_t ip;
	std::stack<uint64_t> stack;

	uint64_t dateofbirth;
	uint64_t errors;

	// just observers so I won't care about dangling pointers
	BaseMemoryType *memPtr;
	Manager *managerPtr;
	Randomizer *randomizerPtr;
	EvoDex *evoDex;

	MemorySpace memoryRecord;
	MemorySpace memoryRecordOffspring;

	uint64_t memHash;

	Metadata metadata;

	bool decode(uint8_t instr, uint64_t address);

	void moveIP(uint8_t lastInstr);

	// instruction set
	bool nop0(uint64_t address);
	bool nop1(uint64_t address);
	bool jmp(uint64_t address);
	bool ifz(uint64_t address);
	bool ifnz(uint64_t address);
	bool fndf(uint64_t address);
	bool fndb(uint64_t address);
	bool call(uint64_t address);
	bool ret(uint64_t address);
	bool zero_a(uint64_t address);
	bool add_a(uint64_t address);
	bool add_b(uint64_t address);
	bool add_c(uint64_t address);
	bool sub_a(uint64_t address);
	bool sub_b(uint64_t address);
	bool sub_c(uint64_t address);
	bool sub_ab(uint64_t address);
	bool movi(uint64_t address);
	bool push_a(uint64_t address);
	bool push_b(uint64_t address);
	bool push_c(uint64_t address);
	bool pop_a(uint64_t address);
	bool pop_b(uint64_t address);
	bool pop_c(uint64_t address);
	bool maloc(uint64_t address);
	bool divide(uint64_t address);
};
