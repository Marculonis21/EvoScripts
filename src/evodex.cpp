#include "evodex.hpp"
#include <iostream>

void EvoDex::insert(const LPU &lpu, LPU::DexEntry dexEntry) {
	auto it = dex.find(lpu);

	if (it == dex.end()) { 
		dex.emplace(lpu, Bucket{dexEntry}); 
		/* auto x = std::cin.get(); */
		return; 
	}

	Bucket bucket = dex[lpu];

	for (auto && entry : bucket) {
		if (entry == dexEntry) { return; }
	}
	/* auto x = std::cin.get(); */

	dex[lpu].push_back(dexEntry);
}
