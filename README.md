*Marek Becvar*

# EvoScripts

Just an idea which has been probably done in different ways (and I can't
remember the name of it). BUT doing it in the C++ with a different flavor seems
like an interesting idea, and working with evolutionary things, you never know
what you are going to get.

---

I want to try making an *evolutionary ecosystem* with different evolving
creatures made out of functions (func for moving, eating, reproducing,
collecting information, etc.). A creature would be a kind of a run-time mutable
class with evolving methods. The idea would be to think of a system allowing to
programmatically evolve code of functions themselves.

First ideas are taking me towards creatures being actuall scripts which would
be parsed. This would allow for more natural *code/dna injection*. The other
option coming in mind is to have some sort of generic function pointer system
which would receive specific complete evolved functions. That might lend
towards some kind of functional approach (hehe Haskell mentioned?).

I would like to think that this approach could evolve interesting behavior in
all the ways in which the function evolution would be allowed. I remember one
of the older projects (*still missing its name...*) did something similar but I
always felt like there was a really big emphasis on code injection (opened
gateways in every creature which could have been exploited by parasites). And
voila, that project breed basically just a really cool parasites which
sabotaged other creatures in interesting ways. 

What if one of the functionalities for the creatures was to have some kind of
immune system, which would have more permissions over other code. What if I
take some biological inspiration and create first couple of eating functions
based on some cellular life. Let's test!

--- 

#### 2024-07-01 -- Lua or not to Lua

Two ideas come to mind. Staying fully in cpp, doing the kind of function
pointer way. How to change the functions? There would need to be some kind of
functional way how functions (parts of functions) are nested to allow for parts
of functions to be changed easily. Other than the implementation details, I
most likely know all what needs to be done for it which seems like a big PLUS.

On the other hand, WHAT ABOUT LUA SCRIPTING? I've read about easy easy ways how
to add Lua scripting to cpp projects. Although, I've never done it but it
doesn't seem that difficult from what I've found and read so far. It would
allow for functions to be created on fly which would make all of it much more
dynamic (function modification, code injection, small changes, ...).

I'm leaning towards Lua, while little scared because I've never done Lua :D.
But I think both approaches are worth a try and I want to give them both a fair
chance!

#### 2024-10-22 13:10 -- Tierra was what I was looking for...

[Tierra](https://faculty.cc.gatech.edu/~turk/bio_sim/articles/tierra_thomas_ray.pdf)
is the inspiration and I just might start with that... better than what I was
doing till this point... just thinking and not doing s***.

The innitial idea was fine, BUT it is probably the best idea to go as low as
possible with the chosen language. Selection of assembler makes me scared, but
there's the challenge! It provides much easier control over the creatures and
the whole sim runtime.

***

Taking notes from above, actually doing custom ISA (instruction set
architecture), sounds like a bunch of pain, BUT it might actually be fun and
enable me doing some nice things different than the original work.

What kind of things would I like to see in my ISA?

 | Imp.                    | Inst./Feat. | Descr.                                                      |
 |-------------------------|-------------|-------------------------------------------------------------|
 | <ul><li>- [x] </li></ul> | Stack       | stack for call and retrieval                                |
 | <ul><li>- [x] </li></ul> | Regs        | a few arbitrary registers                                   |
 | <ul><li>- [x] </li></ul> | IP          | instruction pointer                                         |
 | <ul><li>- [x] </li></ul> | Jump        | jump to point                                               |
 | <ul><li>- [x] </li></ul> | Find        | find closest template                                       |
 | <ul><li>- [x] </li></ul> | Call        | function call                                               |
 | <ul><li>- [x] </li></ul> | Ret         | ret from function call                                      |
 | <ul><li>- [x] </li></ul> | If          | some conditioned jump (based on test register values only?) |
 | <ul><li>- [x] </li></ul> | Add         | +1 to reg                                                   |
 | <ul><li>- [x] </li></ul> | Sub         | -1 to reg                                                   |
 | <ul><li>- [x] </li></ul> | Reg Sub     | (reg = rega - regb) calc might be necessary                 |
 | <ul><li>- [ ] </li></ul> | Move        | move data between registers (MAYBE ONLY WITH STACK???)      |
 | <ul><li>- [x] </li></ul> | Move        | move data via indirect addressing (only for some regs?)     |
 | Additional              | ----        | ----------------------------------------------------------- |
 | <ul><li>- [x] </li></ul> | Zero_Reg    | reset some reg to zero                                      |

***

#### 2024-11-01 14:55 -- To Become a Rustling or not to become a Rustling

I'm seriously thinking about doing this project in Rust as well! I'm not sure
if I'm proficient enough... BUT it woudl be a great exercise plus I see a bunch
of places where I would love the syntax of Rust to help, Result values, return
enum types and stuff like that! Might want to try to finish this in C++ and do
a test run rewriting it into Rust. Might be fun!

#### 2024-11-04 14:12 -- The value of an instruction

I'm thinking about implementing (after first tests) the value for instructions.
That would make some instructions more expensive than others (e.g. the
allocation should cost much more than adding 1 to a register). This could drive
more optimization out of species!

#### 2024-11-06 18:15 -- Process loop conundrum

What and how to loop processes without much of my intervention to their
*natural* way of being. With some brainstorming (thank you A.), the idea NOW is
to loop processes to their first address in two cases.

One would be if a process is at the end of its instructions i.e. just processed
the instuction at memory space corresponding to its assigned memory space, and
is now moving to another. 

The other one would be if at any point in instruction processing fetch returns
*BLANK* instruction i.e. free address inside the memory, which we are not able
to process.

#### 2024-11-10 18:17 -- Memory allocation algorithms

It seems really helpful to add some memory allocation algorithms. I'm probably
not gonna play around with it that much, implementing something like a Best
Fit! 

One problem that comes to my mind is that I would like to have some sort of
spacial relationship between mother/daughter. Memory should try to provide
memory space for mother cell in her neighborhood (offspring should not be breed
on the other side of the memory space if it is possible).

#### 2024-11-19 13:20 -- Allocation algorithms 

Last time I've added and tested the mother-centered firstfit algorithm. Now I
have to try to do proper LPU switching and running multiple of them at the same
time in larger chunk of memory. 

I'm missing some sort of mutation and some sort of penalizer for faulty
processes.

#### 2025-01-22 21:30 -- Rust maybe

Exams done and now I'm all for following this project! I've tried doing a few
changes, still creatures are spawning over each other at the moment... I'm
thinking about doing this thing in Rust. I like the necessary testing and
return values, which would ease the process of connecting more segments. Or I'm
actually pretty close with this version, so it may just need to iron out a few
problem here and there... We'll see. The rust version is speaking to me!

#### 2025-08-06 11:51 -- Well that was a break

I haven't done much with the project through the semester but I still have it
sitting in my mind. I would like to continue the work now. Ther have been a few
tests of mine trying some project in Rust and I didn't really like it atm
(maybe a skill issue). But I will be continuing the work in C++ and see where
that lands me. I would like to finish this hopefully shortly, because of other
sim project in mind. 

From what I remember, I am missing some type of memory cleaner (the killer
robot) and maybe some changes to how programs are structured in the map. Tests
and that's probably it... We'll see how far off I am xD
