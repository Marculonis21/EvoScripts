#include "lpu_pool.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

void LPUPool::addLPU(LPUHandle predecessor, BaseMemoryType *memPtr, Manager *managerPtr, MemorySpace &&memoryRecord, uint64_t dateofbirth) {

	LPUHandle handle{nextID++};
	auto lpu = std::make_unique<LPU>(handle, memPtr, managerPtr, std::move(memoryRecord), dateofbirth);
	lpuPool.emplace(handle, std::move(lpu));

	if (handleQueue.size() == 0) {
		std::cout << "ADDING FIRST LPU" << std::endl;
		handleQueue.push_back(handle);
		return;
	}

	auto iter = std::find(handleQueue.begin(), handleQueue.end(), predecessor);
	assert(iter != handleQueue.end() && "I hope predecessor always exists somewhere");

	std::cout << "INSERTING LPU " << std::endl;
	std::cout << "pred id: " << predecessor.id << std::endl;

	std::cout << "QUEUE: ";
	for (int i = 0; i < handleQueue.size(); ++i) {
		std::cout << handleQueue[i].id << ", ";
	}
	std::cout << std::endl;

	handleQueue.insert(iter+1, handle);

	std::cout << "AFTER INSERTION: ";
	for (int i = 0; i < handleQueue.size(); ++i) {
		std::cout << handleQueue[i].id << ", ";
	}
	std::cout << std::endl;

	/* auto x = std::cin.get(); */
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
	std::vector<LPUHandle> dead;
	for (size_t i = 0; i < handleQueue.size(); ++i) {
		if (!get(handleQueue[i])) {
			dead.push_back(handleQueue[i]);
		}
	}

	handleQueue.erase(std::remove_if(handleQueue.begin(), handleQueue.end(), 
							  [dead](LPUHandle x){
								  return std::find(dead.begin(), dead.end(), x) != dead.end();
							  }), handleQueue.end());

	std::cout << "HandleQueue clear procedure: " << dead.size() << " elements cleared" << std::endl;
	auto x = std::cin.get();
}

void LPUPool::process(size_t lpuSteps) {
	LPU* lpu;

	for (size_t i = 0; i < handleQueue.size(); ++i) {
		lpu = get(handleQueue[i]);
		if (!lpu) { continue; }

		std::cout << "Processing ID: " << handleQueue[i].id << std::endl;
		for (size_t _ = 0; _ < lpuSteps; ++_) {
			lpu->step();
		}
	}
}
