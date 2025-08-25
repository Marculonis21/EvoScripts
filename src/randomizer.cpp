#include "randomizer.hpp"

void Randomizer::process() {
	// update the random values
	randomRes.update();

	if (randomRes.crr_memory <= CRR_MEMORY_RATE) {

		std::uniform_int_distribution<uint64_t> memPos(0, memPtr->memory.size()-1);
		std::uniform_int_distribution<uint8_t> bitPos(0, 4);
		uint64_t pos = memPos(randomRes.engine);
		uint8_t bit = bitPos(randomRes.engine);

		// flip n-th bit
		memPtr->memory[pos] ^= ((uint8_t)1 << bit); 
	}
}

uint8_t Randomizer::cp_instr_process(uint8_t original) {
	// fail
	if (randomRes.cp_instr > CP_INSTR_RATE) { return original; }

	// else
	std::uniform_int_distribution<uint8_t> instrDistr(0, 0x1a);
	return instrDistr(randomRes.engine);
}
