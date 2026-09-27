#include "memoryCleaner.hpp"
#include <cstdint>
#include <limits>
#include <vector>

#include "lpu.hpp"
#include "manager.hpp"

ErrorFirstCleanerStrategy::ErrorFirstCleanerStrategy(Manager *managerPtr) {
	this->managerPtr = managerPtr;
}

// bool ErrorFirstCleanerStrategy::clean(LPUHandle caller) const {
// 	auto popErrors = managerPtr->selectLPUs<uint64_t>(
// 			[](LPU* lpu) -> uint64_t{
// 				return lpu->errorCount();
// 			});

// 	// assert(popErrors.size() > 1 && "We need at least something to clean here!");
// 	if(popErrors.size() <= 1) { return false; }

// 	uint64_t mostErrors = std::numeric_limits<uint64_t>::min();
// 	LPUHandle worstHandle;

// 	for (uint64_t i = 0; i < popErrors.size(); ++i) {
// 		if (popErrors[i].first == caller) {
// 			continue;
// 		}

// 		// if it lives shorter and has the same number of errors than it
// 		// deserves to die
// 		if (popErrors[i].second >= mostErrors) {
// 			mostErrors = popErrors[i].second;
// 			worstHandle = popErrors[i].first; 
// 		}
// 	}

// 	managerPtr->removeLPU(worstHandle);
// 	return true;
// }

bool ErrorFirstCleanerStrategy::clean(LPUHandle caller) const {
    uint64_t maxErrors = 0;
    LPUHandle worstHandle;
    bool found = false;

    managerPtr->forEachLPU([&](LPUHandle handle, const LPU &lpu) {
        if (handle == caller) return;

        uint64_t err = lpu.errorCount();

        // worse or (same and younger)
        if (!found || err > maxErrors || (err == maxErrors && handle.id > worstHandle.id)) {
            maxErrors = err;
            worstHandle = handle;
            found = true;
        }
    });

    if (!found) return false;

    managerPtr->removeLPU(worstHandle);
    return true;
}

