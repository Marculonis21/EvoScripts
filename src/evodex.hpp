#pragma once

#include "lpu.hpp"
#include "lpu_addons.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Bucket = std::vector<LPU::Metadata>;
struct EvoDex {
    void insert(const LPU &parent, const LPU &offspring, const LPU::Metadata &metadata);
    bool exists(const LPUHandle &handle);
	std::unordered_map<LPU::Instructions, Bucket, LPU::Instructions::Hash> dex;
	std::unordered_set<LPUHandle, LPUHandle::Hash> recordedHandles;
};
