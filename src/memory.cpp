#include "memory.hpp"
#include "allocStrategy.hpp"
#include "lpu_addons.hpp"
#include "memoryCleaner.hpp"
#include "memoryHelperStructs.hpp"
#include "randomizer.hpp"
#include "profiler.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

BaseMemoryType::BaseMemoryType(uint64_t size,
							   std::unique_ptr<AllocStrategy> allocStrategy,
							   std::unique_ptr<MemoryCleanerStrategy> cleanerStrategy)
	: allocatedSpaces(size), 
	  allocStrategy(std::move(allocStrategy)), 
	  cleanerStrategy(std::move(cleanerStrategy)){

	this->memory = std::vector<uint8_t>(size, 0);
}

std::optional<uint8_t> BaseMemoryType::fetch(uint64_t address) const {
	if (address < memory.size()) {
		return memory[address];
	}

	return std::nullopt;
}
uint8_t BaseMemoryType::fetchUnsafe(uint64_t address) const {
	return memory[address];
}

uint64_t BaseMemoryType::getMemorySize() const { return memory.size(); }

/*
 * Find suitable place to allocate memorySpace of size `size` around `address`
 * and save and return allocated memorySpace
 */
std::optional<MemorySpace> BaseMemoryType::allocate(uint64_t address,
													uint64_t size,
													LPUHandle caller) {
	ProfileScope p(PROF_ALLOC);
	// simple constraints on soup size
	if (memory.size() < size*0.25) {
		return std::nullopt;
	}

	// base case with fresh memory - fresh but two boundary spaces, are alright
	// TODO:: what about checking if user does not specify some bullshit first
	// memory space... ought to happen at least once...
	if (allocatedSpaces.size() == 2) {
		auto space = MemorySpace{address, size};
		allocatedSpaces.insert(space);
		return space;
	}

	std::optional<MemorySpace> space;
	// WARN:CAREFUL NOW!
	while (1) {
		/* space.reset(); */
		space = allocStrategy->allocate(allocatedSpaces, address, size);
		if (space.has_value()) { break; }

		bool cleaned = cleanerStrategy->clean(caller);
		if (!cleaned) { return std::nullopt; }
	}

	if (space) {
		allocatedSpaces.insert(*space);
	}

	return space;
}

TemplateInfo BaseMemoryType::loadInTemplate(uint64_t address) const {
	uint64_t res = 0;

	for (uint8_t offset = 0; ; ++offset) {
		// start loading after the original instr address
		switch (fetch(address + 1 + offset).value_or(0)) {
			case 0x01: // nop0
				break;
			case 0x02:
				res |= 1 << offset; // nop1
				break;
			default:
				return TemplateInfo{address, res, offset};
		}
	}
}

bool BaseMemoryType::validateTemplate(const TemplateInfo &pattern, const MemorySpace &lpuSpace) const {
	return pattern.patternSize >= min_pattern_size && (!lpuSpace.contains(pattern.start) || lpuSpace.contains(pattern.start+pattern.patternSize));
}

std::optional<MatchSearchHit> BaseMemoryType::scanTemplateRange(
		uint64_t rangeStart,
		uint64_t rangeEnd,
		const TemplateInfo &pattern,
		uint64_t originAddress) const
{
	// find pattern complement
	uint64_t mask = (1ULL << pattern.patternSize) - 1;
	uint64_t expected = (~pattern.pattern) & mask;

	uint64_t check = 0;
	uint8_t offset = 0;
	uint8_t instr = 0;

	MatchSearchHit bestMatch{0, std::numeric_limits<uint64_t>::max()};

	auto dist = [](uint64_t x, uint64_t y) -> uint64_t {
        return (x > y) ? (x - y) : (y - x);
    };

	auto evaluateMatch = [&](uint64_t i) {
		if (offset == pattern.patternSize && check == expected) {

			// Dist from origin to start of the template -- finding closest
			auto matchDistance = dist(originAddress, i - offset);
			if (matchDistance < bestMatch.distance) {
				bestMatch = MatchSearchHit{i - offset, matchDistance};
			}
		}
		check = 0;
		offset = 0;
	};

	for (uint64_t i = rangeStart; i < rangeEnd; ++i) {
		instr = fetchUnsafe(i);

		if (instr == 0x01 || instr == 0x02) {
			check |= (instr == 0x02 ? 1ULL : 0ULL) << offset;
			offset++;
		} else {
			evaluateMatch(i);
		}
	}

	// in case template was at the end of range
	evaluateMatch(rangeEnd);

	if (bestMatch.distance < std::numeric_limits<uint64_t>::max()) {
		return bestMatch;
	}
	return std::nullopt;
}

MatchResult BaseMemoryType::matchTemplateBackward(uint64_t address, const MemorySpace &lpuSpace) const {
	return matchTemplateWorker(TemplateMatchMode::BACKWARD, address, lpuSpace);
}

MatchResult BaseMemoryType::matchTemplateForward(uint64_t address, const MemorySpace &lpuSpace) const {
	return matchTemplateWorker(TemplateMatchMode::FORWARD, address, lpuSpace);
}

MatchResult BaseMemoryType::matchTemplate(uint64_t address, const MemorySpace &lpuSpace) const {
	return matchTemplateWorker(TemplateMatchMode::BIDIRECTIONAL, address, lpuSpace);
}

MatchResult BaseMemoryType::matchTemplateWorker(TemplateMatchMode mode, uint64_t address, const MemorySpace &lpuSpace) const {
	ProfileScope p(PROF_TEMPLATE_MATCH);
	TemplateInfo pattern = loadInTemplate(address);

	if (!validateTemplate(pattern, lpuSpace)) {
		return MatchResult::FAIL();
	}

	std::optional<MatchSearchHit> match;

	uint64_t start, end;
	switch (mode) {
		case TemplateMatchMode::FORWARD:
			start = std::min(getMemorySize(), address + pattern.patternSize + 1);
			end = std::min(getMemorySize(), start + searchSize);
			match = scanTemplateRange(start, end, pattern, address);
			break;
		case TemplateMatchMode::BACKWARD:
			end = std::min(getMemorySize(), address); 
			start = (end < searchSize) ? 0 : end - searchSize;
			match = scanTemplateRange(start, end, pattern, address);
			break;
		case TemplateMatchMode::BIDIRECTIONAL:
			start = address + pattern.patternSize + 1;
			end = std::min(getMemorySize(), start + searchSize);
			auto forwardMatch = scanTemplateRange(start, end, pattern, address);

			start = (address < searchSize) ? 0 : address - searchSize;
			auto backwardMatch = scanTemplateRange(start, address, pattern, address);

			if (forwardMatch.value_or(MatchSearchHit{}).distance < backwardMatch.value_or(MatchSearchHit{}).distance) {
				match = forwardMatch;
			}
			else {
				match = backwardMatch;
			}

			break;
	}

	if (!match.has_value()) {
		return MatchResult::FAIL();
	}

	return MatchResult::SUCCESS((*match).address);

	// TODO: could select with probability based on distance from vector of hit
	// matches
}


bool BaseMemoryType::write(const MemorySpace &lpuSpace, uint64_t address,
						   uint8_t payload) {
	if (!lpuSpace.contains(address))
		return false;

	memory[address] = payload;
	return true;
}

bool BaseMemoryType::copy(const MemorySpace &lpuSpace,
						  const MemorySpace &lpuSpaceOffspring,
						  uint64_t addressFrom, uint64_t addressTo,
						  Randomizer *randomizer) {
	ProfileScope p(PROF_COPY);
	if (!lpuSpace.contains(addressTo) && !lpuSpaceOffspring.contains(addressTo)) { return false; }
	if (addressFrom >= memory.size()) { return false; }

	memory[addressTo] = !randomizer ? memory[addressFrom] : randomizer->instructionCopyStep(memory[addressFrom]);

	return true;
}
