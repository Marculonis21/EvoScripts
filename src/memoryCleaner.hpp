#pragma once

#include "lpu_addons.hpp"
#include "memoryHelperStructs.hpp"

class Manager;

class MemoryCleanerStrategy {
  public:
	MemoryCleanerStrategy() = default;
	virtual ~MemoryCleanerStrategy() = default;

	virtual MemorySpace clean(LPUHandle caller) const = 0;

};

class ErrorFirstCleanerStrategy : public MemoryCleanerStrategy{
  public:
	ErrorFirstCleanerStrategy(Manager *managerPtr);
	virtual MemorySpace clean(LPUHandle caller) const override;

  protected:
	Manager* managerPtr;
};
