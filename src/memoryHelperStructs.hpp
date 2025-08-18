#pragma once

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

	MemorySpace() = default;
	MemorySpace(uint64_t, uint64_t);

	uint64_t start = 0;
	uint64_t size = 0;

	bool contains(uint64_t address) const;
	bool collides(MemorySpace other) const;
	bool isEmpty() const;

	// necessary for erase-remove
	bool operator==(const MemorySpace& other) const {
		return start == other.start && size == other.size;
	}

	static MemorySpace EMPTY() { return MemorySpace(0,0); }
};

class AllocSpacesContainer {
	using msVec = std::vector<MemorySpace>;

  public:
	using const_iterator = msVec::const_iterator;

	AllocSpacesContainer(uint64_t memorySize);

	void insert(MemorySpace inserted);
	void erase(MemorySpace erased);

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
