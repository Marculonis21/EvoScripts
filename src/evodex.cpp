#include "evodex.hpp"

void EvoDex::insert(const LPU &parent, const LPU &offspring, const LPU::Metadata &metadata) {
	// Instruction comparison
	// We are looking for individuals which are able to replicate themselves correctly
	if (!(parent == offspring)) { return; }

	auto it = dex.find(parent);

	if (it == dex.end()) { 
		dex.emplace(parent, Bucket{metadata}); 
		return; 
	}

	Bucket &bucket = it->second;

	for (auto &entry : bucket) {
		if (entry == metadata) { 
			entry.occurence += 1;
			return; 
		}
	}

	dex[parent].push_back(metadata);
}

bool EvoDex::exists(const LPUHandle &handle) {
	for (auto && pair : dex) {
		if (handle == pair.first.getHandle()) {
			return true;
		}
	}

	return false;
}
