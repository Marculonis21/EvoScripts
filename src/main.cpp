#include "manager.hpp"
#include <cstdio>

int main(int argc, char *argv[]) {
    SimConfig config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            std::cout << "EvoScripts - Artificial Life Simulation\n\n"
                << "Usage: ./EvoScripts [OPTIONS]\n\n"
                << "Options:\n"
                << "  -a, --ancestor <path>     Ancestor script (default: ancestors/tester.es)\n"
                << "  -m, --memory <size>       Virtual RAM soup size (default: 10000)\n"
                << "  -i, --iterations <n>      Number of iterations (default: 0 = infinite)\n"
                << "  -s, --steps <n>           Steps per creature per turn (default: 100)\n"
                << "  -p, --snapshot <n>        Interval for writing snapshots (default: 1000)\n"
                << "  --out <path>              Output file path (default: evodex.json)\n"
                << "  -P, --profile             Enable performance subsystem breakdown\n"
                << "  -h, --help                Show this help message\n";
            return 0;
        } else if ((arg == "-a" || arg == "--ancestor") && i + 1 < argc) {
            config.ancestorFile = argv[++i];
        } else if ((arg == "-m" || arg == "--memory") && i + 1 < argc) {
            config.memorySize = std::stoull(argv[++i]);
        } else if ((arg == "-i" || arg == "--iterations") && i + 1 < argc) {
            config.maxIterations = std::stoull(argv[++i]);
        } else if ((arg == "-s" || arg == "--steps") && i + 1 < argc) {
            config.stepsPerOrganism = std::stoull(argv[++i]);
        } else if ((arg == "-p" || arg == "--snapshot") && i + 1 < argc) {
            config.snapshotInterval = std::stoull(argv[++i]);
        } else if (arg == "--out" && i + 1 < argc) {
            config.outputFile = argv[++i];
        } else if (arg == "--profile" || arg == "-P") {
            config.enableProfiling = true;
        } else {
            std::cerr << "Unknown argument: " << arg << " (see --help)\n";
            return 1;
        }
    }

    Manager manager(config);
    manager.sim();
    return 0;
}
