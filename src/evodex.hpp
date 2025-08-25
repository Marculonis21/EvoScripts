#pragma once

#include "lpu.hpp"
#include <unordered_map>
#include <vector>

using Bucket = std::vector<LPU::DexEntry>;
struct EvoDex {
    void insert(const LPU &lpu, LPU::DexEntry dexEntry);
	std::unordered_map<LPU, Bucket, LPU::Hash> dex;
};
