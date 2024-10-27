#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "memorySpace.hpp"
#include <memory>

class CLIvisualizer {
  public:
	CLIvisualizer(std::shared_ptr<BaseMemoryType> memPtr);

    void print();
    void print(uint64_t start, uint64_t end);

  private:
	std::shared_ptr<BaseMemoryType> memPtr;
};

#endif
