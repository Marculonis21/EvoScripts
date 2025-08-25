#pragma once

#include "lpu.hpp"
#include <unordered_map>
#include <vector>

using Bucket = std::vector<LPU::Metadata>;
struct EvoDex {
    void insert(const LPU &parent, const LPU &offspring, const LPU::Metadata &metadata);
	std::unordered_map<LPU, Bucket, LPU::Hash> dex;
};
