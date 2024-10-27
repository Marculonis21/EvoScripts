#include <memory>
#include <string>

#include "lpu.hpp"
#include "memorySpace.hpp"
#include "visualizer.hpp"

int main(int argc, char *argv[]) {
	auto memPtr = std::make_shared<Basic1D>(40);

	CLIvisualizer visualizer(memPtr);
	LPU lpu{memPtr, 0};

	visualizer.print();
}
