#ifndef MEM_HELPER_STRUCTGS_HPP
#define MEM_HELPER_STRUCTGS_HPP
#include <cstddef>
#include <cstdint>
#include <vector>

struct TemplateInfo {
	uint64_t pattern = 0;
	uint8_t patternSize = 0;
};

struct MatchResult {
	bool success = false;
	uint64_t address = 0;
};

struct MatchSearchHit {
	uint64_t address = 0;
	float distance = 0.0;
};

struct MemorySpace {
	uint64_t start = 0;
	uint64_t size = 0;

	bool contains(uint64_t address) const;
	bool collides(MemorySpace other) const;
};

class AllocSpacesContainer {
	using msVec = std::vector<MemorySpace>;

  public:
	using const_iterator = msVec::const_iterator;

	AllocSpacesContainer(uint64_t memorySize);

	void insert(MemorySpace inserted);

	const_iterator begin() const { return allocatedSpaces.begin(); };
	const_iterator end() const { return allocatedSpaces.end(); };
	const_iterator cbegin() const { return allocatedSpaces.cbegin(); };
	const_iterator cend() const { return allocatedSpaces.cend(); };
	const MemorySpace &at(size_t i) const { return allocatedSpaces.at(i); };
	size_t size() const { return allocatedSpaces.size(); };

	int findInsertIndex(const MemorySpace &testSpace) const;

  private:
	int fitBinarySearch(const MemorySpace &testSpace, int low, int high) const;
	msVec allocatedSpaces;
};

#endif
