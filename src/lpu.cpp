#include "lpu.hpp"
#include "lpu_addons.hpp"
#include "memory.hpp"
#include "manager.hpp"
#include <iostream>
#include <cstdio>
#include <optional>
#include <string>

LPU::LPU(BaseMemoryType *memPtr, Manager *managerPtr, MemorySpace memoryRecord) {
	this->memPtr = memPtr;
	this->managerPtr = managerPtr;
	this->memoryRecord = memoryRecord;
	ip = memoryRecord.start;
	memoryRecordOffspring = MemorySpace{0,0};

	regA = uint64_t();
	regB = uint64_t();
	regC = uint64_t();
}

/* operator std::string() const; */
LPU::operator std::string() const {
	std::string output;
	output += "LPU with memoryRecord (start: " + std::to_string(memoryRecord.start) + ", length: " +std::to_string(memoryRecord.size)+")\n";
	output += " - offspring memoryRecord " + (memoryRecordOffspring.size != 0 ? "(start: " + std::to_string(memoryRecordOffspring.start) + ", length: " +std::to_string(memoryRecord.size)+")" : "NONE") + "\n";
	output += " IP: " + std::to_string(ip) + "\n";
	output += " Regs: " + std::to_string(regA) + ", "+ std::to_string(regB) + ", " + std::to_string(regC) + "\n";
	output += " Stack top: " + (!stack.empty() ? std::to_string(stack.top()) : "EMPTY") + "\n";

	return output;
}

void LPU::moveIP(uint8_t lastInstr) {
	// necessary!
	ip += 1;

	// if at the end of own memory space - roll back to the start 
	// if fetched a None instruction - roll back
	if (ip == memoryRecord.start + memoryRecord.size || 
		lastInstr == (uint8_t)Instr::None) {

		ip = memoryRecord.start;
	}
}

bool LPU::step() {
	std::cout << "fetch:" << ip << std::endl;
	std::optional<uint8_t> fetchedInstr = memPtr->fetch(ip);
	if (!fetchedInstr.has_value()) {
		// THIS IS WRONG AND SHOULD BE PUNISHED
		
		ip = memoryRecord.start;
		return false;
	}

	std::cout << instrToStringMap.at(fetchedInstr.value()) << std::endl;
	bool result = decode(fetchedInstr.value(), ip);

	// please for the love of GOD do not forget to add 1 to instruction pointer at the end...
	moveIP(fetchedInstr.value());

	return result;
}

bool LPU::decode(uint8_t instr, uint64_t address) {
	switch (instr) {
		case (uint8_t)Instr::nop0:   return nop0(address);
		case (uint8_t)Instr::nop1:   return nop1(address);
		case (uint8_t)Instr::jmp:    return jmp(address);
		case (uint8_t)Instr::ifz:    return ifz(address);
		case (uint8_t)Instr::ifnz:   return ifnz(address);
		case (uint8_t)Instr::fndf:   return fndf(address);
		case (uint8_t)Instr::fndb:   return fndb(address);
		case (uint8_t)Instr::call:   return call(address);
		case (uint8_t)Instr::ret:    return ret(address);
		case (uint8_t)Instr::zero_a: return zero_a(address);
		case (uint8_t)Instr::add_a:  return add_a(address);
		case (uint8_t)Instr::add_b:  return add_b(address);
		case (uint8_t)Instr::add_c:  return add_c(address);
		case (uint8_t)Instr::sub_a:  return sub_a(address);
		case (uint8_t)Instr::sub_b:  return sub_b(address);
		case (uint8_t)Instr::sub_c:  return sub_c(address);
		case (uint8_t)Instr::sub_ab: return sub_ab(address);
		case (uint8_t)Instr::movi:   return movi(address);
		case (uint8_t)Instr::push_a: return push_a(address);
		case (uint8_t)Instr::push_b: return push_b(address);
		case (uint8_t)Instr::push_c: return push_c(address);
		case (uint8_t)Instr::pop_a:  return pop_a(address);
		case (uint8_t)Instr::pop_b:  return pop_b(address);
		case (uint8_t)Instr::pop_c:  return pop_c(address);
		case (uint8_t)Instr::maloc:  return maloc(address);
		case (uint8_t)Instr::divide: return divide(address);
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
	MatchResult result = memPtr->matchTemplate(address);
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
	MatchResult result = memPtr->matchTemplateForward(address);
	if (!result.success) return false;

	stack.push(result.address);
	return true;
}

bool LPU::fndb(uint64_t address) {
	MatchResult result = memPtr->matchTemplateBackward(address);
	if (!result.success) return false;

	stack.push(result.address);
	return true;
}

/*
 * OK SO, call and return functions at last...
 *
 * Call finds corresponding function which should be started, pushes the IP onto
 * the stack, then jumps IP to the function
 */
bool LPU::call(uint64_t address) {
	MatchResult result = memPtr->matchTemplate(address);
	if (!result.success) return false;

	// push ip+1 so ret (top-1) can possibly get back to original address and
	// next ip is going to be the one following the call instruction
	stack.push(ip+1);

	// jump to address-1 so the next step runs on result.address
	ip = result.address-1;

	return true;
}

/*
 * Return won't be taking care of no function stackframes!
 * Let them figure out what to do or how to use it!
 *
 * return pops address from the stack and returns to that address
 * (can be wrong if function messes the stackframe/return outside of function)
 *
 * function fails on call with an empty stack
 */
bool LPU::ret(uint64_t address) {
	if (stack.empty()) return false;
	// return to address -1 so it can be processed next
	ip = stack.top() - 1;
	stack.pop();

	return true;
}

/*
 * reset register A
 */
bool LPU::zero_a(uint64_t address) {
	regA = 0;
	return true;
}

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

	std::optional<MemorySpace> allocatedSpace = memPtr->allocate(address, regC);
	if(!allocatedSpace) {
		return false;
	}

	memoryRecordOffspring = allocatedSpace.value();
	regC = memoryRecordOffspring.start;

	return true;
}

bool LPU::divide(uint64_t address) {
	// offspring must be at least allocated
	if (memoryRecordOffspring.size == 0) return false;

	managerPtr->addLpu(memoryRecordOffspring);

	memoryRecordOffspring = MemorySpace{0,0}; // reset memoryRecordOffspring

	return true;
}
