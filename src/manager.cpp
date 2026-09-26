#include "manager.hpp"
#include "esParser.hpp"
#include "lpu.hpp"
#include "memory.hpp"
#include "memoryHelperStructs.hpp"
#include "visualizer.hpp"
#include "profiler.hpp"
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
	Profiler::get().enabled = config.enableProfiling;

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

static std::string formatRate(double rate) {
	char buf[64];
	if (rate >= 1e6) {
		std::snprintf(buf, sizeof(buf), "%.2f MStep/s", rate / 1e6);
	} else if (rate >= 1e3) {
		std::snprintf(buf, sizeof(buf), "%.1f KStep/s", rate / 1e3);
	} else {
		std::snprintf(buf, sizeof(buf), "%.0f Step/s", rate);
	}
	return std::string(buf);
}

void Manager::sim() {
	using clock = std::chrono::steady_clock;

	auto tLast = clock::now();
	double currentSPS = 0;
	uint64_t stepSamples = 0;

	LPU* lpu;
	for (iterationCounter = 0; 
		 config.maxIterations == 0 || iterationCounter < config.maxIterations; 
		 ++iterationCounter) {

		if (iterationCounter % 100 == 0) {
			ProfileScope p(PROF_GRAVES);
			lpuPopulation.clearGraves();
		}
		if (iterationCounter % config.snapshotInterval == 0) {
			ProfileScope p(PROF_SNAPSHOT);
			visualizer->print(lpuPopulation);
		}

		for (size_t i = 0; i < lpuPopulation.queueSize(); ++i) {
			lpu = lpuPopulation.getQueue(i);
			if (!lpu) { continue; }

			ProfileScope p(PROF_EXEC);
			for (size_t _ = 0; _ < config.stepsPerOrganism; ++_) {
				{
					ProfileScope pr(PROF_RANDOMIZER);
					randomizer->step();
				}
				lpu->step();
			}
			
			stepSamples += config.stepsPerOrganism;
		}

		auto tNow = clock::now();
		double elapsed = std::chrono::duration<double>(tNow - tLast).count();

		if (elapsed >= 0.25) {
			currentSPS = static_cast<double>(stepSamples) / elapsed;
			stepSamples = 0;
			tLast = tNow;
		}

		if (iterationCounter % 100 == 0) {
			printf("Iteration %lu | Population: %zu | Speed: %s\n",
				   iterationCounter,
				   lpuPopulation.aliveSize(),
				   formatRate(currentSPS).c_str());
		}
	}

	std::cout << "Simulation complete. Saving final snapshot to " << config.outputFile << std::endl;
	{
		ProfileScope p(PROF_SNAPSHOT);
		visualizer->print(lpuPopulation);
	}

	if (config.enableProfiling) {
		Profiler::get().printReport();
	}
}
