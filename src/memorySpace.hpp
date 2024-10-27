#ifndef MEMORYSPACE_HPP
#define MEMORYSPACE_HPP

#include <cstdint>

class BaseMemoryType {
  public:
    virtual uint8_t fetch(uint64_t address) const = 0;
    virtual uint64_t allocate(uint64_t startAddress, uint64_t size) const = 0;
};

class Basic1D : public BaseMemoryType {
  public:
    Basic1D(uint64_t size);
    uint8_t fetch(uint64_t address) const override;
    uint64_t allocate(uint64_t startAddress, uint64_t address) const override;

  private:
    uint8_t *memory;
};

#endif
