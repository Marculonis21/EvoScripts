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
#include <chrono>

using namespace std::chrono_literals;

Manager::Manager(SimConfig simConfig) {
	this->iterationCounter = 0;
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
	return lpuPopulation.addLPU(predecessor, observers, std::move(newMemoryRecord), iterationCounter);
}

void Manager::removeLPU(LPUHandle handle) {
	LPU* lpu = lpuPopulation.get(handle);
	if (!lpu) {
		return; // Handle already gone or invalid
	}

	auto [rec_main, rec_off] = lpuPopulation.get(handle)->getMemRecords();

	lpuPopulation.removeLPU(handle);

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
	std::chrono::time_point last = std::chrono::system_clock::now();
	std::chrono::time_point now = std::chrono::system_clock::now();
	float step_per_seconds = 0;
	uint64_t step_counter = 0;

	LPU* lpu;
	for (iterationCounter = 0; 
		 config.maxIterations == 0 || iterationCounter < config.maxIterations; 
		 ++iterationCounter) {

		printf("Iteration %lu | Population: %zu | Steps/s:  %.1f\n", iterationCounter, lpuPopulation.aliveSize(), step_per_seconds);
		if (iterationCounter % 100 == 0) { lpuPopulation.clearGraves(); }
		if (iterationCounter % config.snapshotInterval == 0) { visualizer->print(lpuPopulation); }

		for (size_t i = 0; i < lpuPopulation.queueSize(); ++i) {
			lpu = lpuPopulation.getQueue(i);
			if (!lpu) { continue; }

			for (size_t _ = 0; _ < config.stepsPerOrganism; ++_) {
				randomizer->process();
				lpu->step();
			}

			step_counter += config.stepsPerOrganism;

			now = std::chrono::system_clock::now();
			if (now - last >= 1s) {
				step_per_seconds = step_counter;

				last = now;
				step_counter = 0;
			}
		}
	}

	std::cout << "Simulation complete. Saving final snapshot to " << config.outputFile << std::endl;
    visualizer->print(lpuPopulation);
}
