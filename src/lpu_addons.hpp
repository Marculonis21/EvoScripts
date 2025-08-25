#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

struct LPUHandle {
	uint64_t id;

	bool operator==(const LPUHandle &other) const {
		return other.id == id;
	}
	struct Hash {
		uint64_t operator()(const LPUHandle &h) const noexcept {
			return std::hash<uint64_t>()(h.id);
		}
	};
};

enum class Instr : uint8_t {
	None   = 0x00,
	nop0   = 0x01,
	nop1   = 0x02,
	jmp    = 0x03,
	ifz    = 0x04,
	ifnz   = 0x05,
	fndf   = 0x06,
	fndb   = 0x07,
	call   = 0x08,
	ret    = 0x09,
	zero_a = 0x0a,
	add_a  = 0x0b,
	add_b  = 0x0c,
	add_c  = 0x0d,
	sub_a  = 0x0e,
	sub_b  = 0x0f,
	sub_c  = 0x10,
	sub_ab = 0x11,
	movi   = 0x12,
	push_a = 0x13,
	push_b = 0x14,
	push_c = 0x15,
	pop_a  = 0x16,
	pop_b  = 0x17,
	pop_c  = 0x18,
	maloc  = 0x19,
	divide = 0x1a,
};

const std::unordered_map<uint8_t, std::string> instrToStringMap { 
	{(uint8_t)Instr::nop0  , "nop0"   },
	{(uint8_t)Instr::nop1  , "nop1"   },
	{(uint8_t)Instr::jmp   , "jmp"    },
	{(uint8_t)Instr::ifz   , "ifz"   },
	{(uint8_t)Instr::ifnz  , "ifnz"   },
	{(uint8_t)Instr::fndf  , "fndf"   },
	{(uint8_t)Instr::fndb  , "fndb"   },
	{(uint8_t)Instr::call  , "call"   },
	{(uint8_t)Instr::ret   , "ret"    },
	{(uint8_t)Instr::zero_a, "zero_a" },
	{(uint8_t)Instr::add_a , "add_a"  },
	{(uint8_t)Instr::add_b , "add_b"  },
	{(uint8_t)Instr::add_c , "add_c"  },
	{(uint8_t)Instr::sub_a , "sub_a"  },
	{(uint8_t)Instr::sub_b , "sub_b"  },
	{(uint8_t)Instr::sub_c , "sub_c"  },
	{(uint8_t)Instr::sub_ab, "sub_ab" },
	{(uint8_t)Instr::movi  , "movi"   },
	{(uint8_t)Instr::push_a, "push_a" },
	{(uint8_t)Instr::push_b, "push_b" },
	{(uint8_t)Instr::push_c, "push_c" },
	{(uint8_t)Instr::pop_a , "pop_a"  },
	{(uint8_t)Instr::pop_b , "pop_b"  },
	{(uint8_t)Instr::pop_c , "pop_c"  },
	{(uint8_t)Instr::maloc , "maloc"  },
	{(uint8_t)Instr::divide, "divide" },
};

const std::unordered_map<std::string, Instr> stringToInstrMap { 
	{"nop0"  , Instr::nop0   },
	{"nop1"  , Instr::nop1   },
	{"jmp"   , Instr::jmp    },
	{"ifz"   , Instr::ifz    },
	{"ifnz"  , Instr::ifnz   },
	{"fndf"  , Instr::fndf   },
	{"fndb"  , Instr::fndb   },
	{"call"  , Instr::call   },
	{"ret"   , Instr::ret    },
	{"zero_a", Instr::zero_a },
	{"add_a" , Instr::add_a  },
	{"add_b" , Instr::add_b  },
	{"add_c" , Instr::add_c  },
	{"sub_a" , Instr::sub_a  },
	{"sub_b" , Instr::sub_b  },
	{"sub_c" , Instr::sub_c  },
	{"sub_ab", Instr::sub_ab },
	{"movi"  , Instr::movi   },
	{"push_a", Instr::push_a },
	{"push_b", Instr::push_b },
	{"push_c", Instr::push_c },
	{"pop_a" , Instr::pop_a  },
	{"pop_b" , Instr::pop_b  },
	{"pop_c" , Instr::pop_c  },
	{"maloc" , Instr::maloc  },
	{"divide", Instr::divide },
};

