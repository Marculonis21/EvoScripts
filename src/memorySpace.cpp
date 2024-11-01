#include "memorySpace.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>
#include <random>

bool memorySpace::contains(uint64_t address) const {
    return start <= address && address < start+size;
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

memorySpace BaseMemoryType::allocate(uint64_t address, uint64_t size) const { 
    std::default_random_engine generator;

    for (int i = 1; i < 10; ++i) {
        std::normal_distribution<double> distribution(0.0, 2*i);
        double fOffset = fabs(distribution(generator));
        uint64_t testOffset = size*fOffset;

        // check backwards
        uint64_t backCheck;
        if (testOffset > address) {
            backCheck = 0;
        }
        else {
            backCheck = address - testOffset;
        }

        // THIS IS FUCKING UGLY!!! TODO: DO IT BETTER
        bool foundOtherBack = false;
        for (int x = backCheck; x < backCheck + size; ++x) {
            // anything else than 0
            if (memory[x]) {
                foundOtherBack = true;
                break;
            }
        }
        if (!foundOtherBack) {
            return memorySpace{backCheck, size};
        }

        // check forwards
        uint64_t forwardCheck;
        if (testOffset + address + size >= getMemorySize()) {
            forwardCheck = getMemorySize() - size;
        }
        else {
            forwardCheck = address + testOffset;
        }
        
        bool foundOtherFront = false;
        for (int x = forwardCheck; x < forwardCheck + size; ++x) {
            // anything else than 0
            if (memory[x]) {
                foundOtherFront = true;
                break;
            }
        }
        if (!foundOtherFront) {
            return memorySpace{forwardCheck, size};
        }
    }

    return memorySpace{0, 0};
}

templateInfo BaseMemoryType::loadInTemplate(uint64_t address) const {
    uint64_t res = 0;
    uint8_t offset = 0;

    while (true) {
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


matchResult BaseMemoryType::matchTemplate(uint64_t address, const templateInfo &pattern) const {
    // TODO: should be refactored and TESTED!

    // vector of hit pairs - start address, distance from start
    std::vector<std::pair<uint64_t, float>> hitVector;

    const int searchSize = 30;

    // search backward
    uint64_t startPoint = address < searchSize ? 0 : address - searchSize;
    std::cout << "backward sp:" <<startPoint << std::endl;

    uint8_t offset = 0;
    uint64_t check = 0;

    uint64_t test = 0;

    uint8_t instr;
     
    for (int i = startPoint; i < address; ++i) {
        instr = fetch(i);

        // section of continuous nops
        if (instr == 0x01 || instr == 0x02) {
            check |= (instr == 0x02 ? 1 : 0) << offset;
            offset++;
        }
        else {
            if (offset >= pattern.patternSize){
                std::cout << "finished pattern at addr" << i << std::endl;
                std::cout << "found pattern: " << check << std::endl;
                test = pattern.pattern ^ check;
                std::cout << "test xor result: " << test << std::endl;
                if (((test + 1) & test) == 0) {
                    // matching pattern found! -- save first pos of template, distance from starting address
                    std::cout << "MATCH" << std::endl;
                    hitVector.emplace_back(i - offset, (address - (i - offset))/float(searchSize));
                }
            }

            check = 0;
            offset = 0;
        }
    }

    // forward
    startPoint = address + pattern.patternSize + 1;
    std::cout << "forward sp:" <<startPoint << std::endl;

    for (int i = startPoint; i < startPoint+searchSize; ++i) {
        instr = fetch(i);

        // section of continuous nops
        if (instr == 0x01 || instr == 0x02) {
            check |= (instr == 0x02 ? 1 : 0) << offset;
            offset++;
        }
        else {
            if (offset >= pattern.patternSize){
                std::cout << "finished pattern at addr" << i << std::endl;
                std::cout << "found pattern: " << check << std::endl;
                test = pattern.pattern ^ check;
                std::cout << "test xor result: " << test << std::endl;
                if (((test + 1) & test) == 0) {
                    // matching pattern found! -- save first pos of template, distance from starting address
                    std::cout << "MATCH" << std::endl;
                    hitVector.emplace_back(i - offset, address - (i - offset));
                }
            }

            check = 0;
            offset = 0;
        }
    }

    // choose some of the hit templates
    
    if (hitVector.size() > 0) return matchResult{false, 0};

    // TODO: CHECK THIS
    std::random_shuffle(hitVector.begin(), hitVector.end());
    return matchResult{true, hitVector[0].first};
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
