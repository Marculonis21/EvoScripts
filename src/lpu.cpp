#include "lpu.hpp"
#include "memorySpace.hpp"

LPU::LPU(const std::shared_ptr<BaseMemoryType> &memPtr, uint64_t ipStart) {
	this->memPtr = memPtr;
	ip = ipStart;
	regA, regB, regC = uint64_t();
}

bool LPU::step() {
	std::cout << "step ip: " << ip << std::endl;
	uint8_t fetchedInstr = memPtr->fetch(ip);

	bool result = decode(fetchedInstr, ip);

	
	/* return true; */
}

bool LPU::decode(uint8_t instr, uint64_t address) {
	switch (instr) {
		default: return false;
	/* case 0x01: */
	/* 	return ; */
	/* case 0x02: */
	/* 	return; */
	/* case 0x03: */
	/* 	return; */
	/* case 0x04: */
	/* 	return; */
	/* case 0x05: */
	/* 	return; */
	/* case 0x06: */
	/* 	return; */
	/* case 0x07: */
	/* 	return; */
	/* case 0x08: */
	/* 	return; */
	}
}

/*
 * nop0, nop1 are no-operation funcs designed for addressing
 */
bool LPU::nop0(uint64_t address) { return true; }
bool LPU::nop1(uint64_t address) { return true; }

/*
 * jmp loads in the following template and jumps to a matching template
 */
bool LPU::jmp(uint64_t address) {
	matchResult result = memPtr->matchTemplate(address, memPtr->loadInTemplate(address));
    
    if (!result.success) return false;

	ip = result.address;
	return true;
}

/*
 * conditional-jump on regA == 0 performs jmp op
 */
bool LPU::cjmp(uint64_t address) {
	if (regA == 0) {
		return jmp(address);
	}

	return true;
}

/*
 * find op loads in a template, finds a matching template and puts its address onto a stack to be used later
 */
bool LPU::find(uint64_t address) {
	matchResult result = memPtr->matchTemplate(address, memPtr->loadInTemplate(address));

	if (!result.success) return false;

	stack.push(result.address);

	return true;
}


bool LPU::call(uint64_t address) {}
bool LPU::ret(uint64_t address) {}

/*
 * simple add_X, sub_X operations on registers
 */
bool LPU::add_a(uint64_t address) {
	regA += 1;
	return true;
}
bool LPU::add_b(uint64_t address) {
	regB += 1;
	return true;
}

bool LPU::sub_a(uint64_t address) {
	regA -= 1;
	return true;
}
bool LPU::sub_b(uint64_t address) {
	regB -= 1;
	return true;
}

/*
 * sub_ab performs subtraction of registers a and b and stores result in c
 */
bool LPU::sub_ab(uint64_t address) {
	regC = regA - regB;
	return true;
}

/* bool LPU::mov(uint64_t address) { */
/* } */

/*
 * move_instruction - does a copy of an instruction at address [regA] to address
 * [regB]
 */
bool LPU::movi(uint64_t address) {
	return memPtr->copy(memoryRecord, regA, regB);
}

bool LPU::push_a(uint64_t address) {}
bool LPU::push_b(uint64_t address) {}
bool LPU::push_c(uint64_t address) {}
bool LPU::pop_a(uint64_t address) {}
bool LPU::pop_b(uint64_t address) {}
bool LPU::pop_c(uint64_t address) {}
bool LPU::maloc(uint64_t address) {}
bool LPU::divide(uint64_t address) {}
