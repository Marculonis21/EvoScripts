#include "manager.hpp"
#include "esParser.hpp"
#include "memorySpace.hpp"
#include "visualizer.hpp"
#include <iostream>
#include <string>

Manager::Manager() {
	memory= std::make_unique<BaseMemoryType>(100);
	lpuPopulation.reserve(10);

    memorySpace ancestorRecord = insertAnimal("ancestors/tester.es");
    if(ancestorRecord.size == 0)
    {
        std::cout << "insert failed" << std::endl;
    }
    else {
        addLpu(ancestorRecord);
    }

    std::cout << std::string(lpuPopulation[0]) << std::endl;
    CLIvisualizer vis(memory.get());
    vis.print();
}

void Manager::addLpu(memorySpace newMemoryRecord) {
	lpuPopulation.emplace_back(memory.get(), this, std::move(newMemoryRecord));
}

void Manager::stepDebug(int lpuId) {
    std::string input;
    std::cout << "Step debug for lpu " << lpuId << std::endl;

    CLIvisualizer vis(memory.get());

    while (true) {
        vis.print();

        for (uint64_t i = lpuPopulation[lpuId].currentIP()-2; i < lpuPopulation[lpuId].currentIP()+3; ++i) {
            std::cout << LPU::decode_tostring(memory->fetch(i)); 
            if(i == lpuPopulation[lpuId].currentIP()) {
                std::cout << "   <--- CURRENT LINE"; 
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << std::string(lpuPopulation[lpuId]) << std::endl;

        std::cout << "\n n + ENTER TO CONTINUE | q + ENTER TO EXIT" << std::endl;
        std::cin >> input;

        if (input == "n") {
            std::cout << "Output: " << std::to_string(lpuPopulation[lpuId].step()) << std::endl;;
            continue;
        }
        else {
            break;
        }
    }
}

memorySpace Manager::insertAnimal(const std::string &filename) {
    std::vector<Instr> ancestorCommands;
    try {
        ancestorCommands = ESParses::parseFile(filename);
    } catch (std::invalid_argument &e) {
        std::cout << "Problem with parsing ancestor file:" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return memorySpace();
    }

    memorySpace mRecord = memory->allocate(memory->getMemorySize()/2,ancestorCommands.size());
    std::cout << std::to_string(mRecord.start) << ": size " << std::to_string(mRecord.size) << std::endl;
    if (mRecord.size == 0) {
        return memorySpace();
    }

    for (int i = 0; i < ancestorCommands.size(); ++i) {
        memory->write(mRecord, mRecord.start+i, (uint8_t)ancestorCommands[i]);
    }

    return mRecord;
}
