#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "lpu.hpp"
#include "memorySpace.hpp"
#include <memory>

class Manager {
  public:
	Manager();

	void addLpu(memorySpace memoryRecord);

	void stepDebug(int lpuId);

  private:
	std::unique_ptr<BaseMemoryType> memory;
	std::vector<LPU> lpuPopulation;

	memorySpace insertAnimal(const std::string &filename);
};

#endif
