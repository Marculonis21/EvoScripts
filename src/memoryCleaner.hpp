#pragma once

#include "lpu_addons.hpp"

class Manager;

class MemoryCleanerStrategy {
  public:
	MemoryCleanerStrategy() = default;
	virtual ~MemoryCleanerStrategy() = default;

	virtual void clean(LPUHandle caller) const = 0;

};

class ErrorFirstCleanerStrategy : public MemoryCleanerStrategy{
  public:
	ErrorFirstCleanerStrategy(Manager *managerPtr);
	virtual void clean(LPUHandle caller) const override;

  protected:
	Manager* managerPtr;
};
