#include "memorySpace.hpp"
#include <cstdint>

Basic1D::Basic1D(uint64_t size) { memory = new uint8_t[size]; }

uint8_t Basic1D::fetch(uint64_t address) const { 
    return 5; 
}
uint64_t Basic1D::allocate(uint64_t startAddress, uint64_t size) const {
    return 5;
}
