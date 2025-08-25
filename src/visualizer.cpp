#include "visualizer.hpp"
#include "lpu.hpp"
#include "lpu_addons.hpp"
#include "lpu_pool.hpp"
#include "memoryHelperStructs.hpp"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

void CLIVisualizer::print(const LPUPool &pool) const {
	std::cout << "---- Memory print out ----" << std::endl;
	std::cout << "- full memory size " << std::to_string(memPtr->getMemorySize()) << std::endl;
	std::cout << "\nLine num | instr hex | instr" << std::endl;

	for (uint64_t i = 0; i < memPtr->getMemorySize(); ++i) {
		uint8_t instr = memPtr->fetch(i).value();
		auto str = LPU::decode_tostring(instr); 
		std::printf("%2d | " ,int(i));
		std::printf("0x%02x | " ,static_cast<int>(instr));
		std::printf("%s", str.c_str());
		std::printf("\n");
	}
}

void TXTFileVisualizer::print(const LPUPool &pool) const {
	std::ofstream file;
	file.open(path);
	assert(file.is_open() && "Problem opening the file for printing");

	auto records = pool.select<std::pair<const MemorySpace, const MemorySpace>>([](LPU* lpu)->std::pair<const MemorySpace, const MemorySpace>{
				return lpu->getMemRecords();
			});

	std::vector<std::tuple<MemorySpace, LPUHandle, bool>> recordsList;
	for (auto && r : records) {
		recordsList.emplace_back(r.second.first, r.first, true);
		if (!r.second.second.isEmpty()) {
			recordsList.emplace_back(r.second.second, r.first, false);
		}
	}

	uint64_t ridx = 0;
	std::sort(recordsList.begin(), recordsList.end(), [](const auto &a, const auto &b){ return std::get<0>(a).start <= std::get<0>(b).start; }); // uff
																																				 
	file << "---- Evo dex ----" << std::endl;
	for (auto && [key, bucket] : evoDexPtr->dex) {

		for (auto && item : bucket) {
			if (item.occurence < 10) { continue; } 

			file << "ENTRY:" << std::endl;
			file << "Occurence: " << item.occurence<< std::endl;
			file << "Handle: " << item.handle.id << " Parent Handle: " << item.parent.id << std::endl;
			file << "DOB: " << item.dateofbirth << std::endl;
			file << "Instructions (lenght: " << item.instructions.vec.size() << ")" << std::endl;

			for (int i = 0; i < item.instructions.vec.size(); ++i) {
				uint8_t instr = item.instructions.vec[i];
				auto str = LPU::decode_tostring(instr); 

				file << int(i) << " | ";
				file << static_cast<int>(instr) << "|";
				file << str.c_str() << std::endl;
			}

			file << std::endl;
		}
	}

	file << "---- Memory print out ----" << std::endl;
	file << "- full memory size " << std::to_string(memPtr->getMemorySize()) << std::endl;
	file << "\nLine num | instr hex | instr" << std::endl;

	for (uint64_t i = 0; i < memPtr->getMemorySize(); ++i) {
		uint8_t instr = memPtr->fetch(i).value();
		auto str = LPU::decode_tostring(instr); 
		file << int(i) << " | ";
		file << static_cast<int>(instr) << "|";
		file << str.c_str();

		MemorySpace ms = std::get<0>(recordsList[ridx]);
		LPUHandle handle = std::get<1>(recordsList[ridx]);
		bool active = std::get<2>(recordsList[ridx]);

		if (ms.start == i) {
			file << " -> MS START ";
			file << "Handle: " << std::to_string(handle.id);
			file << " Active " << std::to_string(active);
		}
		if (ms.start + ms.size-1 == i) {
			file << " -> ~ MS END ~";
			file << "Handle: " << std::to_string(handle.id);
			file << " Active " << std::to_string(active);
			ridx += 1;
		}

		file << std::endl;


	}

	file.close();
}
