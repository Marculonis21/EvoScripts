#include "randomizer.hpp"

Randomizer::CosmicRays::CosmicRays(uint64_t memorySize) : memPosDistr(0, memorySize-1) {}

void Randomizer::CosmicRays::operator()(BaseMemoryType *mem, std::mt19937 &rng) {
	uint64_t pos = memPosDistr(rng);
	uint8_t bit = bitPosDistr(rng);

	mem->memory[pos] ^= (static_cast<uint8_t>(1) << bit);
}

uint8_t Randomizer::InstructionFailure::operator()(std::mt19937 &rng) {
	return instrDistr(rng);
}

Randomizer::Randomizer(BaseMemoryType *memPtr) : memPtr(memPtr), engine(std::random_device{}()), cosmicRays(memPtr->getMemorySize()) {}

void Randomizer::step() {
	if (realDistr(engine) <= CRR_MEMORY_RATE) {
		cosmicRays(memPtr, engine);
	}
}

uint8_t Randomizer::instructionCopyStep(uint8_t original) {
	if (realDistr(engine) <= CP_INSTR_RATE) {
		return instructionFailure(engine);
	}
	return original;
}
