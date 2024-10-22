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

 | Imp. | Inst.     | Descr.                                                  |
 |------|-----------|---------------------------------------------------------|
 | 🔴   | Jump      | jump to point                                           |
 | 🔴   | Call      | function call                                           |
 | 🔴   | Stack     | stack for jumping and retrieval |
 | 🔴   | Ret       | ret from function call                                  |
 | 🔴   | If        | some conditioned jump (based on test register value?)   |
 | 🔴   | Find(dir) | search for some point in code - get address to register |
 | 🔴   | Add       | +1 to reg                                               |
 | 🔴   | Sub       | -1 to reg                                               |
 | 🔴   | Move      | move data between registers                             |

