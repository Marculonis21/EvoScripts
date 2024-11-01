#include <memory>
#include <string>

#include "lpu.hpp"
#include "memorySpace.hpp"
#include "visualizer.hpp"

int main(int argc, char *argv[]) {
	auto memPtr = std::make_shared<BaseMemoryType>(40);

	/* LPU lpu{memPtr, }; */

	/* CLIvisualizer visualizer(memPtr); */
	/* visualizer.print(); */
}
