#pragma once 

#include "lpu_pool.hpp"
#include "memory.hpp"
#include "memoryHelperStructs.hpp"
#include "visualizer.hpp"
#include "randomizer.hpp"
#include <memory>
#include <vector>

class Manager {
  public:
	Manager();

	void addLPU(LPUHandle predecessor, MemorySpace &&memoryRecord);
	void removeLPU(LPUHandle handle);

	template<typename T>
	std::vector<std::pair<LPUHandle, T>> selectLPUs(std::function<T(LPU*)> selector) {
		return lpuPopulation.select(selector);
	}
	void sim();

  private:
	std::unique_ptr<BaseMemoryType> memory;
	LPUPool lpuPopulation;

	std::unique_ptr<VisualizerStrategy> visualizer;

	std::unique_ptr<Randomizer> randomizer;

	MemorySpace insert(const std::string &filename);

	uint64_t stepCounter;
	uint64_t lpuIDCounter;
};
