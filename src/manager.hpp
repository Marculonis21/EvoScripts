#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "memorySpace.hpp"
#include <memory>

class LPU;

class Manager {

  public:
	Manager();

	void addLpu(memorySpace memoryRecord);

  private:
	std::unique_ptr<BaseMemoryType> memory;
	std::vector<LPU> lpuPopulation;
};

#endif
