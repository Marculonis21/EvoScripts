#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "memorySpace.hpp"
#include <memory>

class CLIvisualizer {
  public:
	CLIvisualizer(BaseMemoryType *memPtr);

	void print();
	void print(uint64_t start, uint64_t end);

  private:
	BaseMemoryType *memPtr;
};

#endif
