#include "evodex.hpp"
#include "lpu_addons.hpp"

void EvoDex::insert(const LPU &parent, const LPU &offspring, const LPU::Metadata &metadata) {
	// Instruction comparison
	// We are looking for individuals which are able to replicate themselves correctly and fully
	if (!(parent.getInstructions() == offspring.getInstructions())) { return; }

	const auto &key = metadata.instructions;
	auto it = dex.find(key);

	// Does not exist yet -> create new species entry and register founder handle
	if (it == dex.end()) { 
		dex.emplace(key, Bucket{metadata}); 
		recordedHandles.insert(metadata.handle);
		return; 
	}

	// Exists -> find where to increase occurence
	Bucket &bucket = it->second;
	for (auto &entry : bucket) {
		if (entry == metadata) { 
			entry.occurence += 1;
			return; 
		}
	}

	bucket.push_back(metadata);
	recordedHandles.insert(metadata.handle);
}

bool EvoDex::exists(const LPUHandle &handle) {
	return recordedHandles.contains(handle);
}
