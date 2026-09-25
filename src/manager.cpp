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

Manager::Manager(SimConfig simConfig) {
	this->stepCounter = 0;
	this->lpuIDCounter = 0;
	this->config = simConfig;

	memory = std::make_unique<BaseMemoryType>(
		config.memorySize, 
		std::unique_ptr<AllocStrategy>(new AllocFirstFit()),
		std::unique_ptr<MemoryCleanerStrategy>(new ErrorFirstCleanerStrategy(this))
	);
	/* lpuPopulation.reserve(36); */

	this->evoDex = std::make_unique<EvoDex>();

	// this->visualizer = std::unique_ptr<VisualizerStrategy>(new TXTFileVisualizer(memory.get(), evoDex.get(), "memOutput.txt"));
	this->visualizer = std::unique_ptr<VisualizerStrategy>(new JSONVisualizer(memory.get(), evoDex.get(), config.outputFile));

	this->randomizer = std::make_unique<Randomizer>(memory.get());

	observers = LPUObservers{this->memory.get(), this, this->randomizer.get(), evoDex.get() };

	MemorySpace ancestorRecord = this->insert(config.ancestorFile);
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
	LPU* lpu = lpuPopulation.get(handle);
	if (!lpu) {
		return; // Handle already gone or invalid
	}

	auto [rec_main, rec_off] = lpuPopulation.get(handle)->getMemRecords();

	lpuPopulation.removeLPU(handle);
	/* std::cout << "Removed lpu (Handle id: " << handle.id << ")" << std::endl; */

	assert(!rec_main.isEmpty() && "We don't know what we are removing?");

	memory->allocatedSpaces.erase(rec_main);
	if (!rec_off.isEmpty()) {
		memory->allocatedSpaces.erase(rec_off);
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
	LPU* lpu;
	for (uint64_t iter = 0; 
		 config.maxIterations == 0 || iter < config.maxIterations; 
		 ++iter) {

		printf("Iteration %lu | Population: %zu\n", iter, lpuPopulation.aliveSize());
		if (iter % 100 == 0) { lpuPopulation.clearGraves(); }
		if (iter % config.snapshotInterval == 0) { visualizer->print(lpuPopulation); }

		for (size_t i = 0; i < lpuPopulation.queueSize(); ++i) {
			lpu = lpuPopulation.getQueue(i);
			if (!lpu) { continue; }

			for (size_t _ = 0; _ < config.stepsPerOrganism; ++_) {
				randomizer->process();
				lpu->step();
			}
		}
	}

	std::cout << "Simulation complete. Saving final snapshot to " << config.outputFile << std::endl;
    visualizer->print(lpuPopulation);
}
