#include "manager.hpp"
#include <cstdint>
#include <cstdio>

std::unordered_map<uint64_t, std::string> testMap;

int main(int argc, char *argv[]) { 
    Manager manager;
    manager.sim();
}
