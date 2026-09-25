#include "lpu_pool.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

LPU* LPUPool::addLPU(LPUHandle predecessor, const LPUObservers &observers, MemorySpace &&memoryRecord, uint64_t dateofbirth) {
	LPUHandle handle{nextID++};
	auto lpu = std::make_unique<LPU>(handle, predecessor, observers, std::move(memoryRecord), dateofbirth);
	lpuPool.emplace(handle, std::move(lpu));

	if (handleQueue.size() == 0) {
		std::cout << "ADDING FIRST LPU" << std::endl;
		handleQueue.push_back(handle);
	}
	else {
		auto iter = std::find(handleQueue.begin(), handleQueue.end(), predecessor);
		assert(iter != handleQueue.end() && "I hope predecessor always exists somewhere");

		handleQueue.insert(iter+1, handle);
	}

	return get(handle);
}


void LPUPool::removeLPU(LPUHandle handle) {
	lpuPool.erase(handle);
}

LPU* LPUPool::get(LPUHandle handle) const {
	auto it = lpuPool.find(handle);

	return it != lpuPool.end() ? it->second.get() : nullptr;
}

/*
 * Remove already dead handles from the queue
 */
void LPUPool::clearGraves() {
	auto erased = std::erase_if(handleQueue, [this](LPUHandle h) { return !get(h); });

	/* std::cout << "HandleQueue clear procedure: " << dead.size() << " elements cleared" << std::endl; */
}

LPU* LPUPool::getQueue(size_t i) const {
	return get(handleQueue[i]);
}

size_t LPUPool::queueSize() const {
	return handleQueue.size();
}

size_t LPUPool::aliveSize() const {
	return lpuPool.count();
}
