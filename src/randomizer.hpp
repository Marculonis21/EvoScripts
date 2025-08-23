#pragma once

#include "memory.hpp"
#include <functional>
#include <random>

struct RandomRes {
	RandomRes() : engine(std::random_device{}()),
		realDistr() {
			this->random = std::bind(realDistr, engine);
		}

	void update() {
		crr_memory = random();
		crr_stack  = random();
		cp_instr   = random();
		ptrn_bind  = random();
	}

	double crr_memory=1;
	double crr_stack=1;
	double cp_instr=1;
	double ptrn_bind=1;

	std::mt19937 engine;
  private:
	std::uniform_real_distribution<double> realDistr;
	std::function<double()> random;
};

class Randomizer {
  public:
	Randomizer(BaseMemoryType *memPtr) : memPtr(memPtr) {};

	void process();
	uint8_t cp_instr_process(uint8_t original);

  private:
	BaseMemoryType *memPtr;
	RandomRes randomRes;

	const double CRR_MEMORY_RATE = 1e-5;
	const double CRR_STACK_RATE  = 1e-6;
	const double CP_INSTR_RATE   = 1e-6;
	const double PTRN_BIND_RATE  = 1e-6;
};
