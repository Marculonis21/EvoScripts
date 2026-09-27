
#include "allocStrategy.hpp"
#include "memoryHelperStructs.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>

// unnamed namespace for helper func
namespace {
	bool fitsBetween(uint64_t size, const MemorySpace &lower, const MemorySpace &upper) {
		assert(upper.start >= lower.start + lower.size && "Corrupted or incorrect ordering of memory spaces!");

		return (upper.start - (lower.start + lower.size)) >= size;
	}
} // namespace

std::optional<MemorySpace> AllocFirstFit::allocate(const AllocSpacesContainer &allocatedSpaces,
												   uint64_t caller, uint64_t size) const {

	size_t middleIndex = allocatedSpaces.findSpaceIndex(caller);

	bool backEnd = false, frontEnd = false;

	// a really weird loop - if we reach both the back and the front end, then
	// we end the loop
	for (int offset = 1; !(backEnd && frontEnd); ++offset) {
		backEnd = false; 
		frontEnd = false;

		int backIndex  = middleIndex - offset;
		int frontIndex = middleIndex + offset;

		if (backIndex >= 0) {
			const MemorySpace &back = allocatedSpaces[backIndex];
			const MemorySpace &next = allocatedSpaces[backIndex + 1];

			if (fitsBetween(size, back, next)) {
				// size of the new allocated space away from the 'next' block start
				return MemorySpace{next.start - size, size};
			}
		}
		else {
			backEnd = true;
		}

		if (frontIndex < allocatedSpaces.size()) {
			const MemorySpace &forward = allocatedSpaces[frontIndex];
			const MemorySpace &prev    = allocatedSpaces[frontIndex - 1];

			if (fitsBetween(size, prev, forward)) {
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
