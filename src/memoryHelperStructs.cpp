#include "memoryHelperStructs.hpp"

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

AllocSpacesContainer::AllocSpacesContainer(uint64_t memorySize) {
    // boundary memorySpaces
    allocatedSpaces.push_back(MemorySpace{0,0});
    allocatedSpaces.push_back(MemorySpace{memorySize,0});
}

void AllocSpacesContainer::insert(MemorySpace inserted) {
	// keeping the invariant of sorted memory spaces so they can be easily
	// traversed and checked during allocation
    
    auto fitIndex = findInsertIndex(inserted);
    allocatedSpaces.insert(allocatedSpaces.cbegin() + fitIndex, inserted);
}

int AllocSpacesContainer::findInsertIndex(const MemorySpace &testSpace) const {
    return fitBinarySearch(testSpace, 0, allocatedSpaces.size()-1);
}

int AllocSpacesContainer::fitBinarySearch(const MemorySpace &testSpace, int low, int high) const {
	// return the index of an element in the sorted array after which the
	// `testSpace` should be inserted to keep the sorted invariant
    
    if (high-low == 0) { 
        if (testSpace.start < allocatedSpaces[low].start) {
            return low;
        }
        else {
            return low+1;
        }
    }

	int middle = low + (high-low)/2;

    if (testSpace.start < allocatedSpaces[middle].start) {
        return fitBinarySearch(testSpace, low, middle);
    }
    else {
        return fitBinarySearch(testSpace, middle+1, high);
    }
}
