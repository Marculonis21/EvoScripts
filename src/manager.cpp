#include "manager.hpp"
#include "allocStrategy.hpp"
#include "esParser.hpp"
#include "memory.hpp"
#include "memoryHelperStructs.hpp"
#include "visualizer.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>

Manager::Manager() {
	memory = std::make_unique<BaseMemoryType>(
		300, 
		std::unique_ptr<AllocStrategy>(new AllocFirstFit()));
	/* lpuPopulation.reserve(10); */

	MemorySpace ancestorRecord = insertAnimal("ancestors/tester.es");
	if (ancestorRecord.size == 0) {
		throw std::invalid_argument("first animal insert failed");
	} else {
		addLpu(ancestorRecord);
	}

	std::cout << std::string(lpuPopulation[0]) << std::endl;
	this->visualizer =
		std::unique_ptr<VisualizerStrategy>(new CLIvisualizer(memory.get()));
}

void Manager::addLpu(MemorySpace newMemoryRecord) {
	lpuPopulation.emplace_back(memory.get(), this, std::move(newMemoryRecord));
	std::cout << "ADDED -> new pop: " << lpuPopulation.size()  << std::endl;
	processQueue.push(&lpuPopulation.back());
}

void Manager::stepDebug(int lpuId) {
	std::string input;
	std::cout << "Step debug for lpu " << lpuId << std::endl;

	bool lastStepRes = true;

	LPU lpu = lpuPopulation[lpuId];

	while (true) {
		this->visualizer->print();
		printf("\n");
		// print moving window around current ip in memory
		for (uint64_t i = lpu.currentIP() - 2; i < lpu.currentIP() + 3; ++i) {
			printf("%2ld| %s", i,
				   LPU::decode_tostring(memory->fetch(i).value_or(0)).c_str());
			if (i == lpu.currentIP()) {
				printf("   <--- CURRENT LINE");
			}
			printf("\n");
		}
		printf("\n%s\n", std::string(lpu).c_str());

		printf("LastStep Result: %d\n", lastStepRes);

		printf("Pop size: %ld\n", lpuPopulation.size());

		if (lpu.currentIP() == 100 || lpu.currentIP() == 123 ||
			lpu.currentIP() == 124 || lpu.currentIP() == 125) {

			auto x = std::cin.get();
			if (x == 'q') {
				break;
			}
		}

		lastStepRes = lpu.step();
	}
}

MemorySpace Manager::insertAnimal(const std::string &filename) {
	std::vector<Instr> ancestorCommands;
	try {
		ancestorCommands = ESParses::parseFile(filename);
	} catch (std::invalid_argument &e) {
		std::cout << "Problem with parsing ancestor file:" << std::endl;
		std::cout << "Error: " << e.what() << std::endl;
		return MemorySpace();
	}

	MemorySpace mRecord =
		memory->allocate(memory->getMemorySize() / 2, ancestorCommands.size())
			.value();
	if (mRecord.size == 0) {
		return MemorySpace();
	}

	for (int i = 0; i < ancestorCommands.size(); ++i) {
		memory->write(mRecord, mRecord.start + i, (uint8_t)ancestorCommands[i]);
	}

	return mRecord;
}

void Manager::sim() {
	const int stepsAllowed = 10;
	processQueue = std::queue<LPU*>();

	uint64_t iter = 0;
	while (1) {
		iter += 1;
		printf("Iteration %lu: pop size %lu\n", iter, lpuPopulation.size());

		// create queue to be processed
        for (auto &&lpu : this->lpuPopulation) {
			processQueue.push(&lpu);
		}


		for (; !processQueue.empty(); processQueue.pop())
		{
			LPU *lpu = processQueue.front();
			std::cout << std::string(*lpu) << std::endl;
			for (int i = 0; i < stepsAllowed; ++i) {
				lpu->step();
				std::cout << "step done" << std::endl;
			}
		}
	}
}
