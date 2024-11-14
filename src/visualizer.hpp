#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "memory.hpp"

class VisualizerStrategy {
  public:
    VisualizerStrategy(BaseMemoryType *memPtr) { this->memPtr = memPtr; };
	virtual void print() const = 0;

  protected:
    BaseMemoryType *memPtr;
};

class CLIvisualizer : public VisualizerStrategy {
  public:
    CLIvisualizer(BaseMemoryType *memPtr) : VisualizerStrategy(memPtr) {}
	void print() const override;
};

#endif
