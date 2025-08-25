#pragma once

#include "evodex.hpp"
#include "lpu_pool.hpp"
#include "memory.hpp"

class VisualizerStrategy {
  public:
    VisualizerStrategy(BaseMemoryType *memPtr, EvoDex *evoDexPtr) { 
      this->memPtr = memPtr; 
      this->evoDexPtr = evoDexPtr; 
    };

	virtual void print(const LPUPool &pool) const = 0;

  protected:
    BaseMemoryType *memPtr;
    EvoDex *evoDexPtr;
};

class CLIVisualizer : public VisualizerStrategy {
  public:
    CLIVisualizer(BaseMemoryType *memPtr, EvoDex *evoDexPtr) : VisualizerStrategy(memPtr, evoDexPtr) {}
	void print(const LPUPool &pool) const override;
};

class TXTFileVisualizer: public VisualizerStrategy {
  public:
    TXTFileVisualizer(BaseMemoryType *memPtr, EvoDex *evoDexPtr, std::string path) : VisualizerStrategy(memPtr, evoDexPtr), path(path) {}
	void print(const LPUPool &pool) const override;
  private: 
    const std::string path;
};
