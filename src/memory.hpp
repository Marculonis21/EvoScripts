#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "allocStrategy.hpp"
#include "memoryHelperStructs.hpp"
#include <memory>
#include <cstdint>
#include <optional>
#include <vector>

class BaseMemoryType {
  public:
	BaseMemoryType(uint64_t size, std::unique_ptr<AllocStrategy> allocStrategy);

	virtual uint8_t fetch(uint64_t address) const;
	virtual std::optional<MemorySpace> allocate(uint64_t address, uint64_t size);

    virtual uint64_t getMemorySize() const;

	virtual bool write(const MemorySpace &lpuSpace, uint64_t addressTo, uint8_t payload);
	virtual bool copy(const MemorySpace &lpuSpace, const MemorySpace &lpuSpaceOffspring, uint64_t addressFrom, uint64_t addressTo);

	virtual MatchResult matchTemplate(uint64_t address) const;
	virtual MatchResult matchTemplateForward(uint64_t address) const;
	virtual MatchResult matchTemplateBackward(uint64_t address) const;

  protected:
	std::vector<uint8_t> memory;
	AllocSpacesContainer allocatedSpaces; 

	std::unique_ptr<AllocStrategy> allocStrategy;

	const int searchSize = 50;

	virtual TemplateInfo loadInTemplate(uint64_t address) const;
	virtual std::vector<MatchSearchHit> findMatchingTemplateForward(uint64_t address, TemplateInfo pattern) const;
	virtual std::vector<MatchSearchHit> findMatchingTemplateBackward(uint64_t address, TemplateInfo pattern) const;
};


#endif
