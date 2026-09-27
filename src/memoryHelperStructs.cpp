#include "memoryHelperStructs.hpp"
#include <cstdint>
#include <algorithm>

MemorySpace::MemorySpace(uint64_t start, uint64_t size) {
    this->start = start;
    this->size = size;
}

bool MemorySpace::contains(uint64_t address) const {
    return start <= address && address < start+size;
}

bool MemorySpace::collides(MemorySpace other) const {
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

bool MemorySpace::isEmpty() const {
    return this->size == 0;
}

AllocSpacesContainer::AllocSpacesContainer(uint64_t memorySize) {
    // push boundary memory spaces on init
    allocatedSpaces.push_back(MemorySpace{0,0});
    allocatedSpaces.push_back(MemorySpace{memorySize,0});
}

void AllocSpacesContainer::insert(MemorySpace inserted) {
    auto it = std::lower_bound(allocatedSpaces.begin(), allocatedSpaces.end(), inserted);

    allocatedSpaces.insert(it, inserted);
}

void AllocSpacesContainer::erase(MemorySpace erased) {
    auto it = std::lower_bound(allocatedSpaces.begin(), allocatedSpaces.end(), erased);

    if (it != allocatedSpaces.end() && *it == erased) {
        allocatedSpaces.erase(it);
    }
}

size_t AllocSpacesContainer::findSpaceIndex(uint64_t address) const {
    /*
	Find memory space index from which an address comes from (used for caller
	finding -- if there is no space on this address it will still return
	something)
    */

	// WARN: UPPER_BOUND
    auto it = std::upper_bound(allocatedSpaces.begin(), allocatedSpaces.end(), address,
            [](uint64_t addr, const MemorySpace &space) {
                return addr < space.start;
            });

    return std::distance(allocatedSpaces.begin(), it) - 1;
}
