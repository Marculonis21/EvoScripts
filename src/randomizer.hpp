#pragma once

#include "memory.hpp"
#include <cstdint>
#include <random>

class Randomizer {
  public:
	struct CosmicRays {
		explicit CosmicRays(uint64_t memorySize);
		void operator()(BaseMemoryType *mem, std::mt19937 &rng);

		private:
		std::uniform_int_distribution<uint64_t> memPosDistr;
		std::uniform_int_distribution<uint8_t> bitPosDistr{0, 4};
	};

	struct InstructionFailure {
		InstructionFailure() = default;
		uint8_t operator()(std::mt19937 &rng);

		private:
		std::uniform_int_distribution<uint8_t> instrDistr{0, 0x1a};
	};

	explicit Randomizer(BaseMemoryType *mem);

	void step();
	uint8_t instructionCopyStep(uint8_t original);

  private:
	BaseMemoryType *memPtr;

	static constexpr double CRR_MEMORY_RATE = 1e-5;
	static constexpr double CP_INSTR_RATE   = 1e-3;

	std::mt19937 engine;
	std::uniform_real_distribution<double> realDistr{0.0, 1.0};

	CosmicRays cosmicRays;
	InstructionFailure instructionFailure;
};
