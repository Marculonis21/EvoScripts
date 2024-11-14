#ifndef ALLOC_STRATEGY_HPP
#define ALLOC_STRATEGY_HPP

#include "memoryHelperStructs.hpp"
#include <cstdint>

class AllocStrategy {
  public:
	virtual ~AllocStrategy() = default;

	/*
	 * Implements strategies to return a MemorySpace that fits into other currently
	 * allocated memory spaces. The returned MemorySpace is not yet inserted into
	 * the allocatedSpaces container.
	 */
	virtual MemorySpace allocate(const AllocSpacesContainer &allocatedSpaces,
					   		     uint64_t caller, uint64_t size) const = 0;
};

class AllocFirstFit : public AllocStrategy {
  public:
	virtual MemorySpace allocate(const AllocSpacesContainer &allocatedSpaces,
							     uint64_t caller, uint64_t size) const override;
};

#endif
