
#include "allocStrategy.hpp"
#include "memoryHelperStructs.hpp"
#include <cstdint>
#include <optional>
#include <iostream>

// unnamed namespace for helper func
namespace {
	bool fitsBetween(uint64_t size, const MemorySpace &lower, const MemorySpace &upper) {
		return (upper.start - (lower.start + lower.size)) >= size;
	}
} // namespace

std::optional<MemorySpace> AllocFirstFit::allocate(const AllocSpacesContainer &allocatedSpaces,
												   uint64_t caller, uint64_t size) const {

	std::cout << "ALLOC FIRST FIT PROCEDURE" << std::endl;
	// middle = insert index - 1 ... find index of current caller cell (find
	// returns next index, so -1)
	
	std::cout << "Current AS:" << std::endl;
	for (int i = 0; i < allocatedSpaces.size(); ++i) {
		auto as = allocatedSpaces.at(i);
		std::cout << "- Pos: " << as.start << " Size: " << as.size << std::endl;
	}
	
	int middleIndex = allocatedSpaces.findInsertIndex(MemorySpace{caller, 0})-1;
	std::cout << "middle index: " << middleIndex << std::endl;

	bool backEnd, frontEnd = false;
	int backIndex, frontIndex;

	// a really weird loop - if we reach both the back and the front end, then
	// we end the loop
	for (int offset = 1; !(backEnd && frontEnd); ++offset) {
		backEnd = false; 
		frontEnd = false;

		int backIndex  = middleIndex - offset;
		int frontIndex = middleIndex + offset;

		if (backIndex >= 0) {
			MemorySpace back = allocatedSpaces.at(backIndex);
			MemorySpace next = allocatedSpaces.at(backIndex + 1);

			if (fitsBetween(size, back, next)) {
				std::cout << "FITS FIRST" << std::endl;
				std::cout << "BackIndex " << backIndex << std::endl;
				std::cout << " - back " << back.start << ", " << back.size << std::endl;
				std::cout << " - next " << next.start << ", " << next.size << std::endl;
				// size of the new allocated space away from the 'next' block start
				return MemorySpace{next.start - size, size};
			}
		}
		else {
			backEnd = true;
		}

		if (frontIndex < allocatedSpaces.size()) {
			MemorySpace forward = allocatedSpaces.at(frontIndex);
			MemorySpace prev    = allocatedSpaces.at(frontIndex - 1);

			if (fitsBetween(size, prev, forward)) {
				std::cout << "FITS END" << std::endl;
				std::cout << "Frontindex " << frontIndex << std::endl;
				std::cout << " - prev"    << prev.start    << ", " << prev.size << std::endl;
				std::cout << " - forward" << forward.start << ", " << forward.size << std::endl;
				// starts right after the end of prev space
				return MemorySpace{prev.start+prev.size, size};
			}
		}
		else {
			frontEnd = true;
		}
	}

	return std::nullopt;
}
