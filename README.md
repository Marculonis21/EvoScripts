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
