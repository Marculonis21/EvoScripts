// start template
nop0
nop0
nop1
// reset all values to zero
zero_a
push_a
push_a
pop_b
pop_c
// find start template and put the address onto a stack
fndb
nop1
nop1
nop0
// find end template and put the address onto a stack
fndf
nop1
nop0
nop0
// get end to a + size of the template, start to b, do sub_ab: a - b = c
pop_a
// offset the end template + dummy statement
add_a
add_a
add_a
add_a
// end of offsetting
pop_b
sub_ab
// c contains the size of THIS (save it for later)
push_c
maloc
// c contains the start of OFFSPRING
// get start of an OFFSPRING to b and start of THIS to a
push_b
push_c
pop_b
pop_a
// get size of THIS back to c
pop_c
// start of copy loop
nop0
nop1
nop0
// copy instr [rega] to [regb]
movi
add_a
add_b
sub_c
// loop condition - if size != 0 jump to start of copy loop
ifnz 
// jmp -- THIS LITTLE SHIT WAS MAKING COPIES OF ITSELF AND JUMPING TO THE COPIES...
fndb
nop1
nop0
nop1
ifnz 
ret // return back to start of copy loop
// end template
nop0
nop1
nop1
//dummy end
zero_a
