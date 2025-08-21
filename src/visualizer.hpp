#pragma once

#include "lpu_pool.hpp"
#include "memory.hpp"

class VisualizerStrategy {
  public:
    VisualizerStrategy(BaseMemoryType *memPtr) { this->memPtr = memPtr; };
	virtual void print(const LPUPool &pool) const = 0;

  protected:
    BaseMemoryType *memPtr;
};

class CLIVisualizer : public VisualizerStrategy {
  public:
    CLIVisualizer(BaseMemoryType *memPtr) : VisualizerStrategy(memPtr) {}
	void print(const LPUPool &pool) const override;
};

class TXTFileVisualizer: public VisualizerStrategy {
  public:
    TXTFileVisualizer(BaseMemoryType *memPtr, std::string path) : VisualizerStrategy(memPtr), path(path) {}
	void print(const LPUPool &pool) const override;
  private: 
    const std::string path;
};
