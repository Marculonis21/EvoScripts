#include "memoryHelperStructs.hpp"
#include <iostream>
#include <string>

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

void AllocSpacesContainer::insert(MemorySpace inserted) {
	// keeping the invariant of sorted memory spaces so they can be easily
	// traversed and checked during allocation
    
    // push first
    if (this->allocatedSpaces.empty()) {
        allocatedSpaces.push_back(inserted);
        return;
    }

    auto fitIndex = fitBinarySearch(inserted, 0, allocatedSpaces.size());
    allocatedSpaces.insert(allocatedSpaces.cbegin() + fitIndex, inserted);
}

bool AllocSpacesContainer::checkMemorySpaceCollision(const MemorySpace &testSpace) const {
	// TODO: COULD be done much better as allocated space could be sorted in the
	// vector based on starting index and then we would need to check only a few
	// surrounding cases
    
    // index of an element preceeding the testSpace
    auto fitIndex = fitBinarySearch(testSpace, 0, allocatedSpaces.size());

    return allocatedSpaces[fitIndex].collides(testSpace);
}

int AllocSpacesContainer::findInsertIndex(const MemorySpace &testSpace) const {
    return fitBinarySearch(testSpace, 0, allocatedSpaces.size());
}

int AllocSpacesContainer::fitBinarySearch(const MemorySpace &testSpace, int low, int high) const {
	// return the index of an element in the sorted array after which the
	// `testSpace` should be inserted to keep the sorted invariant
    
    std::cout << "Bin search " << low << ":" << high << std::endl;

    if (high-low == 0) { 
        std::cout << "    Bin search return " << low << std::endl;
        return low; 
    }

	int middle = low + (high-low)/2;

    if (testSpace.start < allocatedSpaces[middle].start) {
        return fitBinarySearch(testSpace, low, middle-1);
    }
    else {
        return fitBinarySearch(testSpace, middle+1, high);
    }
}
