#include "memorySpace.hpp"
#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

Basic1D::Basic1D(uint64_t size) { memory = std::vector<uint8_t>(size); }

uint8_t Basic1D::fetch(uint64_t address) const { 
    return memory[address];
}
uint64_t Basic1D::allocate(uint64_t startAddress, uint64_t size) const {
    return 0;
}

templateInfo BaseMemoryType::loadInTemplate(uint64_t address) const {
    uint64_t res = 0;
    uint8_t offset = 0;

    while (true) {
        offset++;
        switch (fetch(address+offset)) {
            case 0x01:                      // nop0 
                break;
            case 0x02: res |= 1 << offset;  // nop1
                break;
            default:   return templateInfo{res, --offset};
        }
    }
}

uint64_t BaseMemoryType::matchTemplate(uint64_t address, templateInfo pattern) const {
    // TODO: should be refactored and TESTED!

    // vector of hit pairs - start address, distance from start
    std::vector<std::pair<uint64_t, int>> hitVector;

    const int searchSize = 5;

    // search backward
    uint64_t startPoint = std::max(address - searchSize,(uint64_t)0);

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
                test = pattern.pattern ^ check;
                if (((test + 1) & test) == 0) {
                    // matching pattern found! -- save first pos of template, distance from starting address
                    hitVector.emplace_back(i - offset, address - (i - offset));
                }
            }

            check = 0;
            offset = 0;
        }
    }

    // forward
    startPoint = address + pattern.patternSize + 1;

    for (int i = startPoint; i < startPoint+searchSize; ++i) {
        instr = fetch(i);

        // section of continuous nops
        if (instr == 0x01 || instr == 0x02) {
            check |= (instr == 0x02 ? 1 : 0) << offset;
            offset++;
        }
        else {
            if (offset >= pattern.patternSize){
                test = pattern.pattern ^ check;
                if (((test + 1) & test) == 0) {
                    // matching pattern found! -- save first pos of template, distance from starting address
                    hitVector.emplace_back(i - offset, address - (i - offset));
                }
            }

            check = 0;
            offset = 0;
        }
    }

    // choose some of the hit templates
    
    // TODO: some sort of exception?
    if (hitVector.size() > 0) { return -1; }

    // TODO: CHECK THIS
    std::random_shuffle(hitVector.begin(), hitVector.end());
    return hitVector[0].first;
}
