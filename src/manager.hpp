#pragma once 

#include "evodex.hpp"
#include "lpu.hpp"
#include "lpu_pool.hpp"
#include "memory.hpp"
#include "memoryHelperStructs.hpp"
#include "visualizer.hpp"
#include "randomizer.hpp"
#include <memory>
#include <utility>
#include <vector>

// Guard these defaults with your life...
struct SimConfig {
	uint64_t memorySize = 10000;
	uint64_t maxIterations = 0; 
	uint64_t stepsPerOrganism = 100;
	uint64_t snapshotInterval = 1000;
	std::string ancestorFile = "ancestors/tester.es";
	std::string outputFile = "evodex.json";
	bool enableProfiling = false;
};

class Manager {
  public:
	Manager(SimConfig config);

	LPU* addLPU(LPUHandle predecessor, MemorySpace &&memoryRecord);
	void removeLPU(LPUHandle handle);

	template<typename Func>
	void forEachLPU(Func &&fn) {
		lpuPopulation.forEach(std::forward<Func>(fn));
	}

	void sim();

  private:
	LPUPool lpuPopulation;

	std::unique_ptr<BaseMemoryType> memory;
	std::unique_ptr<VisualizerStrategy> visualizer;
	std::unique_ptr<Randomizer> randomizer;
	std::unique_ptr<EvoDex> evoDex;

	LPUObservers observers;

	MemorySpace insert(const std::string &filename);

	uint64_t iterationCounter;
	uint64_t lpuIDCounter;

	SimConfig config;
};
