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
	uint64_t address;
};

struct memorySpace {
	uint64_t start;
	uint64_t size;

	bool contains(uint64_t address) const;
};

class BaseMemoryType {
  public:
	virtual uint8_t fetch(uint64_t address) const = 0;

	virtual bool write(const memorySpace &lpuSpace, uint64_t addressTo, uint8_t payload);
	virtual bool copy(const memorySpace &lpuSpace, uint64_t addressFrom, uint64_t addressTo);

	virtual templateInfo loadInTemplate(uint64_t address) const;
	virtual matchResult matchTemplate(uint64_t address,
			const templateInfo &pattern) const;

    virtual uint64_t getMemorySize() const;

  protected:
	std::vector<uint8_t> memory;
};

class Basic1D : public BaseMemoryType {
  public:
	Basic1D(uint64_t size);
	uint8_t fetch(uint64_t address) const override;
};

#endif
