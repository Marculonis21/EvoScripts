#pragma once

#include "lpu.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

class LPUPool {
  public:
	  LPU* addLPU(LPUHandle predecessor, const LPUObservers &observers, MemorySpace &&memoryRecord, uint64_t dateofbirth);
	  void removeLPU(LPUHandle handle);
	  LPU* get(LPUHandle handle) const;
	  void clearGraves();

	  LPU* getQueue(size_t i) const;
	  size_t queueSize() const;

	  template<typename T>
	  std::vector<std::pair<LPUHandle, T>> select(std::function<T(LPU*)> selector) const {
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
