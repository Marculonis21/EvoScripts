#include "manager.hpp"

Manager::Manager() {
	memory= std::make_unique<BaseMemoryType>(40);
	lpuPopulation.reserve(10);
}

void Manager::addLpu(memorySpace newMemoryRecord) {
	lpuPopulation.emplace_back(memory.get(), this, std::move(newMemoryRecord));
}
