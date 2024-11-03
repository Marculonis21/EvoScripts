#include "memorySpace.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <random>

bool memorySpace::contains(uint64_t address) const {
    return start <= address && address < start+size;
}

bool memorySpace::collides(memorySpace other) const {
    // 4 options:
    //  - this starts outside and ends inside other
    //  - this starts inside and ends outside other
    //  - this starts outside and ends outside other
    //  - this starts inside and ends inside other

    return ((this->start <= other.start && this->start+this->size > other.start) ||
            (other.start <= this->start && other.start+other.size > this->start) ||
            (this->start <= other.start && this->start+this->size >= other.start+other.size) ||
            (other.start <= this->start && other.start+other.size >= this->start+this->size));
}

BaseMemoryType::BaseMemoryType(uint64_t size) { 
    memory = std::vector<uint8_t>(size, 0); 
}

uint8_t BaseMemoryType::fetch(uint64_t address) const { 
    return memory[address];
}

uint64_t BaseMemoryType::getMemorySize() const { 
    return memory.size();
}

bool BaseMemoryType::checkMemorySpaceCollisions(const memorySpace &testSpace) const {
	// TODO: COULD be done much better as allocated space could be sorted in the
	// vector based on starting index and then we would need to check only a few
	// surrounding cases

    for (auto && other : allocatedSpaces) {
        if(other.collides(testSpace)) {
            return true;
        }
    }

    return false;
}

/*
 * Find suitable place to allocate memorySpace of size `size` around `address`
 * and save and return allocated memorySpace
 */
memorySpace BaseMemoryType::allocate(uint64_t address, uint64_t size) { 
    std::default_random_engine generator;
    std::cout << "ALLOCATE PROCEDURE" << std::endl;

    memorySpace allocatedSpace;
    memorySpace testSpace{0, size};

    for (int i = 1; i < 10; ++i) {
        std::cout << "ALLOCATE PROCEDURE loop " << i << std::endl;

        std::normal_distribution<double> distribution(0, i);
        double fOffset = fabs(distribution(generator));
        uint64_t testOffset = size*fOffset;

        std::cout << "testOffset " << std::to_string(testOffset) << std::endl;

        // check backwards
        uint64_t backCheck;
        if (testOffset > address) {
            backCheck = 0;
        }
        else {
            backCheck = address - testOffset;
        }

        std::cout << "backCheck start: " << std::to_string(backCheck) << std::endl;
        testSpace.start = backCheck;
        if (!checkMemorySpaceCollisions(testSpace)) {
            allocatedSpace = testSpace;
            break;
        }

        // check forwards
        uint64_t forwardCheck;
        if (testOffset + address + size >= getMemorySize()) {
            forwardCheck = getMemorySize() - size;
        }
        else {
            forwardCheck = address + testOffset;
        }

        std::cout << "forwardCheck start: " << std::to_string(forwardCheck) << std::endl;
        testSpace.start = forwardCheck;
        if (!checkMemorySpaceCollisions(testSpace)) {
            allocatedSpace = testSpace;
            break;
        }
    }


    if (allocatedSpace.size != 0) {
        allocatedSpaces.push_back(allocatedSpace);
    }

    return allocatedSpace;
}

templateInfo BaseMemoryType::loadInTemplate(uint64_t address) const {
    uint64_t res = 0;
    uint8_t offset = 0;

    while (true) {
        std::cout << "loadInTemplate address: " << address+1+offset << std::endl;
        // start loading after the original instr address
        switch (fetch(address+1+offset)) {
            case 0x01:                      // nop0 
                break;
            case 0x02: res |= 1 << offset;  // nop1
                break;
            default:   return templateInfo{res, offset};
        }

        offset++;
    }
}

std::vector<matchSearchHit> BaseMemoryType::findMatchingTemplateForward(uint64_t address, templateInfo pattern) const {
    std::vector<matchSearchHit> hitVector;

    // forward
    uint64_t startPoint = address + pattern.patternSize + 1;

    uint8_t offset = 0;
    uint64_t check = 0;
    uint64_t test = 0;
    uint8_t instr;

    for (uint64_t i = startPoint; i < startPoint+searchSize; ++i) {
        instr = fetch(i);

        // section of continuous nops
        if (instr == 0x01 || instr == 0x02) {
            check |= (instr == 0x02 ? 1 : 0) << offset;
            offset++;
        }
        else {
            check |=  1 << offset;
            if (offset >= pattern.patternSize) {
                std::cout << "forward match - check " << std::to_string(check) << std::endl;
                test = pattern.pattern ^ check; // xor check
                std::cout << "forward match - test " << std::to_string(test) << std::endl;
                // we didn't turn off any bits
                if (test > check && ((test + 1) & test) == 0) {
                    // matching pattern found! -- save first pos of template, distance from starting address
                    // TODO: dunno why this has to pass the whole object back...
                    hitVector.emplace_back(matchSearchHit{i - offset, (address - (i - offset))/float(searchSize)});
                    std::cout << "forward match - hit add " << std::to_string(i - offset) << ", " << std::to_string((address - (i - offset))/float(searchSize))  << std::endl;
                }
            }

            check = 0;
            offset = 0;
        }
    }

    return hitVector;
}

std::vector<matchSearchHit> BaseMemoryType::findMatchingTemplateBackward(uint64_t address, templateInfo pattern) const {
    std::vector<matchSearchHit> hitVector;

    uint64_t startPoint = address < searchSize ? 0 : address - searchSize;

    uint8_t offset = 0;
    uint64_t check = 0;
    uint64_t test = 0;
    uint8_t instr = 0;
     
    for (uint64_t i = startPoint; i < address; ++i) {
        instr = fetch(i);

        // section of continuous nops
        if (instr == 0x01 || instr == 0x02) {
            check |= (instr == 0x02 ? 1 : 0) << offset;
            offset++;
        }
        else {
            if (offset >= pattern.patternSize){
                test = pattern.pattern ^ check; // xor test
                std::cout << "backward match - check " << std::to_string(check) << std::endl;
                std::cout << "backward match - test " << std::to_string(test) << std::endl;
                if (test > check && ((test + 1) & test) == 0) {
                    // matching pattern found! -- save first pos of template, distance from starting address
                    hitVector.emplace_back(matchSearchHit{i - offset, (address - (i - offset))/float(searchSize)});
                    std::cout << "backward match - hit add " << std::to_string(i - offset) << ", " << std::to_string((address - (i - offset))/float(searchSize))  << std::endl;
                }
            }

            check = 0;
            offset = 0;
        }
    }

    return hitVector;
}

matchResult BaseMemoryType::matchTemplateBackward(uint64_t address) const {
    templateInfo pattern = loadInTemplate(address);
    std::cout << "backward - pattern: " << std::to_string(pattern.pattern) << std::endl;
    if (pattern.patternSize == 0) return matchResult{false, 0};

    std::vector<matchSearchHit> hitVector = findMatchingTemplateBackward(address, pattern);
    if (hitVector.size() == 0) return matchResult{false, 0};

    // TODO: CHECK THIS
    // TODO: select with probability based on distance
    std::random_shuffle(hitVector.begin(), hitVector.end());
    return matchResult{true, hitVector[0].address};
}

matchResult BaseMemoryType::matchTemplateForward(uint64_t address) const {
    templateInfo pattern = loadInTemplate(address);
    std::cout << "forward - pattern: " << std::to_string(pattern.pattern) << std::endl;
    if (pattern.patternSize == 0) return matchResult{false, 0};

    std::vector<matchSearchHit> hitVector = findMatchingTemplateForward(address, pattern);
    if (hitVector.size() == 0) return matchResult{false, 0};

    // TODO: CHECK THIS
    std::random_shuffle(hitVector.begin(), hitVector.end());
    return matchResult{true, hitVector[0].address};
}

/*
 * Bidirectional template match
 */
matchResult BaseMemoryType::matchTemplate(uint64_t address) const {
    templateInfo pattern = loadInTemplate(address);
    std::cout << "matchTemplate - pattern: " << std::to_string(pattern.pattern) << std::endl;
    if (pattern.patternSize == 0) return matchResult{false, 0};

    auto hVb = findMatchingTemplateBackward(address, pattern);
    auto hVf = findMatchingTemplateForward(address, pattern);
    std::vector<matchSearchHit> hitVector(hVb.size() + hVf.size()); 

    hitVector = hVb;
    hitVector.insert(hitVector.end(), hVf.begin(), hVf.end());
    if (hitVector.size() == 0) return matchResult{false, 0};

    // TODO: CHECK THIS
    std::random_shuffle(hitVector.begin(), hitVector.end());
    return matchResult{true, hitVector[0].address};
}

bool BaseMemoryType::write(const memorySpace &lpuSpace, uint64_t address, uint8_t payload) {
    if (!lpuSpace.contains(address)) return false;

    memory[address] = payload;
    return true;
}

bool BaseMemoryType::copy(const memorySpace &lpuSpace, const memorySpace &lpuSpaceOffspring, uint64_t addressFrom, uint64_t addressTo) {
    if (!lpuSpace.contains(addressTo) && 
        !lpuSpaceOffspring.contains(addressTo)) {
        return false;
    }

    memory[addressTo] = memory[addressFrom];
    return true;
}
