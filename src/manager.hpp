#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "lpu.hpp"
#include "memory.hpp"
#include "visualizer.hpp"
#include <memory>
#include <queue>

class Manager {
  public:
	Manager();

	void addLpu(MemorySpace memoryRecord);

	void stepDebug(int lpuId);
	void sim();

  private:
	std::unique_ptr<BaseMemoryType> memory;
	std::vector<LPU> lpuPopulation;
	std::queue<LPU*> processQueue;
	std::unique_ptr<VisualizerStrategy> visualizer;

	MemorySpace insertAnimal(const std::string &filename);
};

#endif
