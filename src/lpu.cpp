#include "lpu.hpp"
#include "memorySpace.hpp"
#include "manager.hpp"

LPU::LPU(BaseMemoryType *memPtr, Manager *managerPtr, memorySpace memoryRecord) {
	this->memPtr = memPtr;
	this->managerPtr = managerPtr;
	this->memoryRecord = memoryRecord;
	ip = memoryRecord.start;
	regA, regB, regC = uint64_t();
}

bool LPU::step() {
	uint8_t fetchedInstr = memPtr->fetch(ip);

	bool result = decode(fetchedInstr, ip);

	// please for the love of GOD do not forget to add 1 to instruction pointer at the end...
	ip += 1;

	return result;
}

bool LPU::decode(uint8_t instr, uint64_t address) {
	switch (instr) {
		default: return false;
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
	matchResult result = memPtr->matchTemplate(address);
    if (!result.success) return false;

	// jump to address-1 so the next step runs on result.address
	ip = result.address-1;
	return true;
}

/*
 * conditional on regC == 0 performs next instr
 * otherwise IP is moved and next is skipped
 */
bool LPU::ifz(uint64_t address) {
	if (regC != 0) {
		ip += 1;
	}

	return true;
}

/*
 * opposite to ifz, on regC != 0 performs next op,
 * otherwise IP +1 and next is skipped
 */
bool LPU::ifnz(uint64_t address) {
	if (regC == 0) {
		ip += 1;
	}

	return true;
}

/*
 * find op loads in a template, finds a matching template and puts its address
 * onto a stack to be used later
 */
bool LPU::fndf(uint64_t address) {
	matchResult result = memPtr->matchTemplateForward(address);
	if (!result.success) return false;

	stack.push(result.address);
	return true;
}

bool LPU::fndb(uint64_t address) {
	matchResult result = memPtr->matchTemplateBackward(address);
	if (!result.success) return false;

	stack.push(result.address);
	return true;
}

/*
 * OK SO, call and return functions at last...
 *
 * Call finds corresponding function which should be started, pushes the IP onto
 * the stack, then all registers A,B,C, then jumps IP to the function
 */
bool LPU::call(uint64_t address) {
	matchResult result = memPtr->matchTemplate(address);
	if (!result.success) return false;

	stack.push(ip);
	push_a(address);
	push_b(address);
	push_c(address);

	// jump to address-1 so the next step runs on result.address
	ip = result.address-1;

	return true;
}

/*
 * Return won't be taking care of no stackframes xD
 * Let them figure it out!
 *
 * so return pops values C, B, A to registers, returning to the original state
 * before return (can be wrong/different if function messes the stackframe),
 * then pops and sets IP
 *
 * if at any incorrect moment the stack empties the function crashes and returns
 * false
 */
bool LPU::ret(uint64_t address) {
	if(!pop_c(address)) return false;
	if(!pop_b(address)) return false;
	if(!pop_a(address)) return false;
	if (stack.empty()) return false;
	ip = stack.top();
	stack.pop();

	return true;
}

/*
 * simple add_X, sub_X operations on registers
 */

bool LPU::zero_a(uint64_t address) {
	regA = 0;
	return true;
}

bool LPU::add_a(uint64_t address) {
	regA += 1;
	return true;
}
bool LPU::add_b(uint64_t address) {
	regB += 1;
	return true;
}
bool LPU::add_c(uint64_t address) {
	regC += 1;
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
bool LPU::sub_c(uint64_t address) {
	regC -= 1;
	return true;
}

/*
 * sub_ab performs subtraction of registers a and b and stores result in c
 */
bool LPU::sub_ab(uint64_t address) {
	regC = regA - regB;
	return true;
}

/*
 * move_instruction - does a copy of an instruction at address [regA] to address
 * [regB]
 */
bool LPU::movi(uint64_t address) {
	return memPtr->copy(memoryRecord, memoryRecordOffspring, regA, regB);
}

bool LPU::push_a(uint64_t address) {
	stack.push(regA);
	return true;
}

bool LPU::push_b(uint64_t address) {
	stack.push(regB);
	return true;
}

bool LPU::push_c(uint64_t address) {
	stack.push(regC);
	return true;
}
bool LPU::pop_a(uint64_t address) {
	if (stack.empty()) return false;

	regA = stack.top();
	stack.pop();
	return true;
}
bool LPU::pop_b(uint64_t address) {
	if (stack.empty()) return false;

	regB = stack.top();
	stack.pop();
	return true;
}
bool LPU::pop_c(uint64_t address) {
	if (stack.empty()) return false;

	regC = stack.top();
	stack.pop();
	return true;
}

/*
 * Get new memorySpace with size from regC (or some other reg???) and return it's address to the same reg 
 */
bool LPU::maloc(uint64_t address) {
	// maloc size has to be bigger than 0
	if (regC == 0) return false;
	// cannot allocate another space when one offspring already exists
	if (memoryRecordOffspring.size != 0) return false;

	memoryRecordOffspring = memPtr->allocate(address, regC);

	if (memoryRecordOffspring.size == 0) return false;

	regC = memoryRecordOffspring.start;

	return true;
}

bool LPU::divide(uint64_t address) {
	// offspring must be at least allocated
	if (memoryRecordOffspring.size == 0) return false;

	managerPtr->addLpu(memoryRecordOffspring);
	memoryRecordOffspring = memorySpace(); // reset memoryRecordOffspring

	return true;
}
