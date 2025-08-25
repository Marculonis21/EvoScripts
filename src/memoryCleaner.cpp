#include "memoryCleaner.hpp"
#include <cstdint>
#include <limits>
#include <vector>

#include "lpu.hpp"
#include "manager.hpp"

ErrorFirstCleanerStrategy::ErrorFirstCleanerStrategy(Manager *managerPtr) {
	this->managerPtr = managerPtr;
}

void ErrorFirstCleanerStrategy::clean(LPUHandle caller) const {
	auto popErrors = managerPtr->selectLPUs<uint64_t>(
			[](LPU* lpu) -> uint64_t{
				return lpu->errorCount();
			});

	assert(popErrors.size() > 1 && "We need at least something to clean here!");

	uint64_t mostErrors = std::numeric_limits<uint64_t>::min();
	LPUHandle worstHandle;

	for (uint64_t i = 0; i < popErrors.size(); ++i) {
		if (popErrors[i].first == caller) {
			continue;
		}

		// if it lives shorter and has the same number of errors than it
		// deserves to die
		if (popErrors[i].second >= mostErrors) {
			mostErrors = popErrors[i].second;
			worstHandle = popErrors[i].first; 
		}
	}

	managerPtr->removeLPU(worstHandle);
}
