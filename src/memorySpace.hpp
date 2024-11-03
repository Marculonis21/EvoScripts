#ifndef MEMORYSPACE_HPP
#define MEMORYSPACE_HPP

#include <cstdint>
#include <vector>

struct templateInfo {
	uint64_t pattern = 0;
	uint8_t patternSize = 0;
};

struct matchResult {
	bool success = false;
	uint64_t address = 0;
};

struct matchSearchHit {
	uint64_t address = 0;
	float distance = 0.0;
};

struct memorySpace {
	uint64_t start = 0;
	uint64_t size = 0;

	bool contains(uint64_t address) const;
	bool collides(memorySpace other) const;
};

class BaseMemoryType {
  public:
	BaseMemoryType(uint64_t size);

	virtual uint8_t fetch(uint64_t address) const;
	virtual memorySpace allocate(uint64_t address, uint64_t size);

    virtual uint64_t getMemorySize() const;

	virtual bool write(const memorySpace &lpuSpace, uint64_t addressTo, uint8_t payload);
	virtual bool copy(const memorySpace &lpuSpace, const memorySpace &lpuSpaceOffspring, uint64_t addressFrom, uint64_t addressTo);

	virtual matchResult matchTemplate(uint64_t address) const;
	virtual matchResult matchTemplateForward(uint64_t address) const;
	virtual matchResult matchTemplateBackward(uint64_t address) const;

  protected:
	std::vector<uint8_t> memory;
	std::vector<memorySpace> allocatedSpaces;

	const int searchSize = 50;

	virtual templateInfo loadInTemplate(uint64_t address) const;
	virtual std::vector<matchSearchHit> findMatchingTemplateForward(uint64_t address, templateInfo pattern) const;
	virtual std::vector<matchSearchHit> findMatchingTemplateBackward(uint64_t address, templateInfo pattern) const;

	virtual bool checkMemorySpaceCollisions(const memorySpace &testSpace) const;
};


#endif
