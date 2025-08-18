#include "manager.hpp"
#include "esParser.hpp"
#include "lpu.hpp"
#include "memory.hpp"
#include "memoryHelperStructs.hpp"
#include "visualizer.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <deque>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

Manager::Manager() {
	this->stepCounter = 0;
	this->lpuIDCounter = 0;

	memory = std::make_unique<BaseMemoryType>(
		300, 
		std::unique_ptr<AllocStrategy>(new AllocFirstFit()),
		std::unique_ptr<MemoryCleanerStrategy>(new ErrorFirstCleanerStrategy(this))
	);
	/* lpuPopulation.reserve(36); */

	MemorySpace ancestorRecord = this->insert("ancestors/tester.es");
	if (ancestorRecord.size == 0) {
		throw std::invalid_argument("first animal insert failed");
	} else {
		addLPU(LPUHandle{}, std::move(ancestorRecord));
	}

	std::cout << std::string(*lpuPopulation.get(LPUHandle{0})) << std::endl;
	this->visualizer =
		std::unique_ptr<VisualizerStrategy>(new CLIvisualizer(memory.get()));
}


void Manager::addLPU(LPUHandle predecessor, MemorySpace &&newMemoryRecord) {
	lpuPopulation.addLPU(predecessor, memory.get(), this, std::move(newMemoryRecord), 0);

	std::cout << "Added new lpu " << std::endl;
}

void Manager::removeLPU(LPUHandle handle) {
	auto records = lpuPopulation.select<std::pair<MemorySpace, MemorySpace>>([handle](LPU* lpu)->std::pair<MemorySpace,MemorySpace>{
			if (lpu->getHandle() == handle) {
				return std::make_pair(lpu->memoryRecord, lpu->memoryRecordOffspring);
			}
			else {
				return std::make_pair(MemorySpace::EMPTY(), MemorySpace::EMPTY());
			}});

	for (int i = 0; i < records.size(); ++i) {
		auto main = records[i].second.first;
		auto off = records[i].second.second;

		if (main.isEmpty()) { continue; }

		memory->allocatedSpaces.erase(main);
		if (!off.isEmpty()) {
			memory->allocatedSpaces.erase(off);
		}
	}

	lpuPopulation.removeLPU(handle);
	std::cout << "Removed lpu (Handle id: " << handle.id << ")" << std::endl;
}

MemorySpace Manager::insert(const std::string &filename) {
	std::vector<Instr> ancestorCommands;
	try {
		ancestorCommands = ESParses::parseFile(filename);
	} catch (std::invalid_argument &e) {
		std::cout << "Problem with parsing ancestor file:" << std::endl;
		std::cout << "Error: " << e.what() << std::endl;
		return MemorySpace::EMPTY();
	}

	MemorySpace mRecord =
		memory->allocate(memory->getMemorySize() / 2, ancestorCommands.size(), LPUHandle{})
			.value();

	if (mRecord.size == 0) {
		return MemorySpace::EMPTY();
	}

	for (int i = 0; i < ancestorCommands.size(); ++i) {
		memory->write(mRecord, mRecord.start + i, (uint8_t)ancestorCommands[i]);
	}

	return mRecord;
}

void Manager::sim() {
	const int stepsAllowed = 10;

	uint64_t iter = 0;
	while (1) {
		iter += 1;
		printf("Iteration %lu: \n", iter);
		/* printf("Iteration %lu: pop size %lu\n", iter, lpuPopulation.size()); */

		lpuPopulation.process(stepsAllowed);
	}
}
