
#include "allocStrategy.hpp"
#include "memoryHelperStructs.hpp"
#include <cstdint>
#include <optional>

// unnamed namespace for helper func
namespace {
bool fitsBetween(uint64_t size, const MemorySpace &m1, const MemorySpace m2) {
	return (m2.start - (m1.start + m1.size)) >= size;
};
} // namespace

std::optional<MemorySpace> AllocFirstFit::allocate(const AllocSpacesContainer &allocatedSpaces,
												uint64_t caller, uint64_t size) const {

	int middleIndex = allocatedSpaces.findInsertIndex(MemorySpace{caller, 0});

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
				// size of the new allocated space away from the 'next' block start
				return MemorySpace{next.start - size, size};
			}
		}
		else {
			backEnd = true;
		}

		if (frontIndex < allocatedSpaces.size()) {
			MemorySpace forward = allocatedSpaces.at(frontIndex);
			MemorySpace prev = allocatedSpaces.at(frontIndex - 1);

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