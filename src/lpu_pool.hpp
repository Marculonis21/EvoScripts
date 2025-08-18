#pragma once

#include "lpu.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

class LPUPool {
  public:
	  void addLPU(LPUHandle predecessor, BaseMemoryType *memPtr, Manager *managerPtr, MemorySpace &&memoryRecord, uint64_t dateofbirth);
	  void removeLPU(LPUHandle handle);
	  LPU* get(LPUHandle handle);
	  void process(size_t lpuSteps);

	  template<typename T>
	  std::vector<std::pair<LPUHandle, T>> select(std::function<T(LPU*)> selector) {
		  std::vector<std::pair<LPUHandle, T>> output;
		  for (size_t i = 0; i < handleQueue.size(); ++i) { 
			  auto handle = handleQueue[i]; 
			  auto lpu = get(handle);
			  if (!lpu) { continue; }

			  T value = selector(lpu);

			  output.emplace_back(handle,value);
		  }

		  return output;
	  }

  private:
	  std::unordered_map<LPUHandle, std::unique_ptr<LPU>, LPUHandle::Hash> lpuPool;
	  std::vector<LPUHandle> handleQueue;

	  uint64_t nextID=0;
};
