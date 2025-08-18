#include "memory.hpp"
#include "allocStrategy.hpp"
#include "lpu_addons.hpp"
#include "memoryCleaner.hpp"
#include "memoryHelperStructs.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

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

uint64_t BaseMemoryType::getMemorySize() const { return memory.size(); }

/*
 * Find suitable place to allocate memorySpace of size `size` around `address`
 * and save and return allocated memorySpace
 */
std::optional<MemorySpace> BaseMemoryType::allocate(uint64_t address,
													uint64_t size,
													LPUHandle caller) {

	std::cout << "ALLOCATION FUNC" << std::endl;
	// base case with fresh memory - fresh but two boundary spaces, are alright
	// TODO:: what about checking if user does not specify some bullshit first
	// memory space... ought to happen at least once...
	if (allocatedSpaces.size() == 2) {
		auto space = MemorySpace{address, size};
		allocatedSpaces.insert(space);
		return space;
	}

	std::optional<MemorySpace> space;
	// CAREFUL NOW!
	while (1) {
		/* space.reset(); */
		space = allocStrategy->allocate(allocatedSpaces, address, size);

		if (space.has_value()) { break; }

		std::cout << "No more space for allocation - calling cleaner" << std::endl;
		cleanerStrategy->clean(caller);
	}
	/* assert(space.has_value() && */
	/* 	   "THERE IS NO MORE SPACE FOR ALLOCATION PROBABLY..."); */

	if (space) {
		std::cout << "INSERTED SOMEWHERE!" << std::endl;
		allocatedSpaces.insert(space.value());
	}
	auto x = std::cin.get();

	return space;
}

TemplateInfo BaseMemoryType::loadInTemplate(uint64_t address) const {
	uint64_t res = 0;
	uint8_t offset = 0;

	while (true) {
		std::cout << "loadInTemplate address: " << address + 1 + offset
				  << std::endl;
		// start loading after the original instr address
		switch (fetch(address + 1 + offset).value()) {
		case 0x01: // nop0
			break;
		case 0x02:
			res |= 1 << offset; // nop1
			break;
		default:
			return TemplateInfo{res, offset};
		}

		offset++;
	}
}

std::vector<MatchSearchHit>
BaseMemoryType::findMatchingTemplateForward(uint64_t address,
											TemplateInfo pattern) const {
	std::vector<MatchSearchHit> hitVector;

	// forward
	uint64_t startPoint = address + pattern.patternSize + 1;

	uint8_t offset = 0;
	uint64_t check = 0;
	uint64_t test = 0;
	uint8_t instr;

	for (uint64_t i = startPoint; i < startPoint + searchSize; ++i) {
		if (!fetch(i).has_value()) { break; }

		instr = fetch(i).value();

		// section of continuous nops
		if (instr == 0x01 || instr == 0x02) {
			check |= (instr == 0x02 ? 1 : 0) << offset;
			offset++;
		} else {
			check |= 1 << offset;
			if (offset >= pattern.patternSize) {
				std::cout << "forward match - check " << std::to_string(check)
						  << std::endl;
				test = pattern.pattern ^ check; // xor check
				std::cout << "forward match - test " << std::to_string(test)
						  << std::endl;
				// we didn't turn off any bits
				if (test > check && ((test + 1) & test) == 0) {
					// matching pattern found! -- save first pos of template,
					// distance from starting address
					// TODO: dunno why this has to pass the whole object back...
					hitVector.emplace_back(
						MatchSearchHit{i - offset, (address - (i - offset)) /
													   float(searchSize)});
					std::cout << "forward match - hit add "
							  << std::to_string(i - offset) << ", "
							  << std::to_string(((i - offset) - address) /
												float(searchSize))
							  << std::endl;
				}
			}

			check = 0;
			offset = 0;
		}
	}

	return hitVector;
}

std::vector<MatchSearchHit>
BaseMemoryType::findMatchingTemplateBackward(uint64_t address,
											 TemplateInfo pattern) const {
	std::vector<MatchSearchHit> hitVector;

	uint64_t startPoint = address < searchSize ? 0 : address - searchSize;

	uint8_t offset = 0;
	uint64_t check = 0;
	uint64_t test = 0;
	uint8_t instr = 0;

	for (uint64_t i = startPoint; i < address; ++i) {
		instr = fetch(i).value();

		// section of continuous nops
		if (instr == 0x01 || instr == 0x02) {
			check |= (instr == 0x02 ? 1 : 0) << offset;
			offset++;
		} else {
			if (offset >= pattern.patternSize) {
				test = pattern.pattern ^ check; // xor test
				std::cout << "backward match - check " << std::to_string(check)
						  << std::endl;
				std::cout << "backward match - test " << std::to_string(test)
						  << std::endl;
				if (test > check && ((test + 1) & test) == 0) {
					// matching pattern found! -- save first pos of template,
					// distance from starting address
					hitVector.emplace_back(
						MatchSearchHit{i - offset, (address - (i - offset)) /
													   float(searchSize)});
					std::cout << "backward match - hit add "
							  << std::to_string(i - offset) << ", "
							  << std::to_string((address - (i - offset)) /
												float(searchSize))
							  << std::endl;
				}
			}

			check = 0;
			offset = 0;
		}
	}

	return hitVector;
}

MatchResult BaseMemoryType::matchTemplateBackward(uint64_t address) const {
	TemplateInfo pattern = loadInTemplate(address);
	std::cout << "backward - pattern: " << std::to_string(pattern.pattern)
			  << std::endl;
	if (pattern.patternSize == 0)
		return MatchResult{false, 0};

	std::vector<MatchSearchHit> hitVector =
		findMatchingTemplateBackward(address, pattern);

	std::cout << "MATCH BACKWARD BACK" << std::endl;
	if (hitVector.size() == 0) {
		return MatchResult{false, 0};
	}

	// TODO: CHECK THIS
	// TODO: select with probability based on distance
	std::random_shuffle(hitVector.begin(), hitVector.end());
	return MatchResult{true, hitVector[0].address};
}

MatchResult BaseMemoryType::matchTemplateForward(uint64_t address) const {
	TemplateInfo pattern = loadInTemplate(address);
	std::cout << "forward - pattern: " << std::to_string(pattern.pattern)
			  << std::endl;
	if (pattern.patternSize == 0)
		return MatchResult{false, 0};

	std::vector<MatchSearchHit> hitVector =
		findMatchingTemplateForward(address, pattern);
	if (hitVector.size() == 0)
		return MatchResult{false, 0};

	// TODO: CHECK THIS
	std::random_shuffle(hitVector.begin(), hitVector.end());
	return MatchResult{true, hitVector[0].address};
}

/*
 * Bidirectional template match
 */
MatchResult BaseMemoryType::matchTemplate(uint64_t address) const {
	TemplateInfo pattern = loadInTemplate(address);
	std::cout << "matchTemplate - pattern: " << std::to_string(pattern.pattern)
			  << std::endl;
	if (pattern.patternSize == 0)
		return MatchResult{false, 0};

	auto hVb = findMatchingTemplateBackward(address, pattern);
	auto hVf = findMatchingTemplateForward(address, pattern);
	std::vector<MatchSearchHit> hitVector(hVb.size() + hVf.size());

	hitVector = hVb;
	hitVector.insert(hitVector.end(), hVf.begin(), hVf.end());
	if (hitVector.size() == 0)
		return MatchResult{false, 0};

	std::random_shuffle(hitVector.begin(), hitVector.end());
	return MatchResult{true, hitVector[0].address};
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
						  uint64_t addressFrom, uint64_t addressTo) {
	if (!lpuSpace.contains(addressTo) &&
		!lpuSpaceOffspring.contains(addressTo)) {
		return false;
	}

	memory[addressTo] = memory[addressFrom];
	return true;
}
