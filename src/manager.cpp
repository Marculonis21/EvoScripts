#include "manager.hpp"
#include "esParser.hpp"
#include "lpu.hpp"
#include "memory.hpp"
#include "memoryHelperStructs.hpp"
#include "visualizer.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <vector>

Manager::Manager() {
	this->stepCounter = 0;
	this->lpuIDCounter = 0;

	memory = std::make_unique<BaseMemoryType>(
		10000, 
		std::unique_ptr<AllocStrategy>(new AllocFirstFit()),
		std::unique_ptr<MemoryCleanerStrategy>(new ErrorFirstCleanerStrategy(this))
	);
	/* lpuPopulation.reserve(36); */

	this->evoDex = std::make_unique<EvoDex>();

	this->visualizer =
		/* std::unique_ptr<VisualizerStrategy>(new CLIVisualizer(memory.get())); */
		std::unique_ptr<VisualizerStrategy>(new TXTFileVisualizer(memory.get(), evoDex.get(), "memOutput.txt"));

	this->randomizer = std::make_unique<Randomizer>(memory.get());

	observers = LPUObservers{this->memory.get(), this, this->randomizer.get(), evoDex.get() };

	MemorySpace ancestorRecord = this->insert("ancestors/tester.es");
	if (ancestorRecord.size == 0) {
		throw std::invalid_argument("first animal insert failed");
	} else {
		addLPU(LPUHandle{}, std::move(ancestorRecord));
	}
}

LPU* Manager::addLPU(LPUHandle predecessor, MemorySpace &&newMemoryRecord) {
	return lpuPopulation.addLPU(predecessor, observers, std::move(newMemoryRecord), 0);

	/* std::cout << "Added new lpu " << std::endl; */
}

void Manager::removeLPU(LPUHandle handle) {
	auto records = lpuPopulation.select<std::pair<MemorySpace, MemorySpace>>([handle](LPU* lpu)->std::pair<MemorySpace,MemorySpace>{
			if (lpu->getHandle() == handle) {
				return lpu->getMemRecords();
			}
			else {
				return std::make_pair(MemorySpace::EMPTY(), MemorySpace::EMPTY());
			}});

	lpuPopulation.removeLPU(handle);
	/* std::cout << "Removed lpu (Handle id: " << handle.id << ")" << std::endl; */

	for (int i = 0; i < records.size(); ++i) {
		auto main = records[i].second.first;
		auto off = records[i].second.second;

		if (main.isEmpty()) { continue; }

		memory->allocatedSpaces.erase(main);
		if (!off.isEmpty()) {
			memory->allocatedSpaces.erase(off);
		}
	}
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
	const int stepsAllowed = 100;

	LPU* lpu;
	for (uint64_t iter = 0; ; ++iter) {
		printf("Iteration %lu \n", iter);
		if (iter % 100 == 0) { lpuPopulation.clearGraves(); }
		if (iter % 2000 == 0) { 
			visualizer->print(lpuPopulation); 
		}

		for (size_t i = 0; i < lpuPopulation.queueSize(); ++i) {
			lpu = lpuPopulation.getQueue(i);
			if (!lpu) { continue; }

			for (size_t _ = 0; _ < stepsAllowed; ++_) {
				randomizer->process();

				lpu->step();
			}
		}
		/* lpuPopulation.process(stepsAllowed); */
	}
}
