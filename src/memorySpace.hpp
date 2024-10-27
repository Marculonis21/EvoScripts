#ifndef MEMORYSPACE_HPP
#define MEMORYSPACE_HPP

#include <cstdint>
#include <vector>

struct templateInfo {
	uint64_t pattern = 0;
	uint8_t patternSize = 0;
};

class BaseMemoryType {
  public:
	virtual uint8_t fetch(uint64_t address) const = 0;
	virtual uint64_t allocate(uint64_t startAddress, uint64_t size) const = 0;

	virtual templateInfo loadInTemplate(uint64_t address) const;
	virtual uint64_t matchTemplate(uint64_t address,
								   templateInfo pattern) const;

    virtual uint64_t getMemorySize() const;

  protected:
	std::vector<uint8_t> memory;
};

class Basic1D : public BaseMemoryType {
  public:
	Basic1D(uint64_t size);
	uint8_t fetch(uint64_t address) const override;
	uint64_t allocate(uint64_t startAddress, uint64_t address) const override;
};

#endif
