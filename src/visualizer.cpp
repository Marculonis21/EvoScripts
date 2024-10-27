#include "visualizer.hpp"
#include "lpu.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

CLIvisualizer::CLIvisualizer(std::shared_ptr<BaseMemoryType> memPtr) {
	this->memPtr = memPtr;
}

void CLIvisualizer::print() {
	std::cout << "---- Memory print out ----" << std::endl;
	std::cout << "- full memory size " << std::to_string(memPtr->getMemorySize()) << std::endl;
	std::cout << "\nLine num | instr hex | instr" << std::endl;

	for (uint64_t i = 0; i < memPtr->getMemorySize(); ++i) {
		uint8_t instr = memPtr->fetch(i);
		auto str = LPU::decode_tostring(instr); 
		std::printf("%2d | " ,int(i));
		std::printf("0x%02x | " ,static_cast<int>(instr));
		std::printf("%s", str.c_str());
		std::printf("\n");
	}
}