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

struct memorySpace {
	uint64_t start = 0;
	uint64_t size = 0;

	bool contains(uint64_t address) const;
};

class BaseMemoryType {
  public:
	BaseMemoryType(uint64_t size);

	virtual uint8_t fetch(uint64_t address) const;

	virtual memorySpace allocate(uint64_t address, uint64_t size) const;

	virtual bool write(const memorySpace &lpuSpace, uint64_t addressTo, uint8_t payload);
	virtual bool copy(const memorySpace &lpuSpace, const memorySpace &lpuSpaceOffspring, uint64_t addressFrom, uint64_t addressTo);

	virtual matchResult matchTemplate(uint64_t address) const;

    virtual uint64_t getMemorySize() const;

  protected:
	virtual templateInfo loadInTemplate(uint64_t address) const;
	std::vector<uint8_t> memory;
};


#endif
