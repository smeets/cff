
[<~ previous](https://github.com/smeets/cff)
[next ~>](obfuscation)

# 1 - introduction

Apply a flattening transform on the control flow that hides the original flow while keeping a strict equivalence in terms of input/output. The core assumptions and observations can be expressed as:

 - The *source code* is compiled into *machine instructions* by the compiler
 - Machine *instructions* can be grouped into *blocks* by jumps & entry points
 - These *blocks* are chained together to form *procedures*
 - A *program* consists of one or more *procedures*
 - The control flow graph is a graph which describes all possible execution paths of a particular *program*, *procedure* or *block*
 - Static analysis of the control flow graph is constructed by finding branches (implicit & explicit) between *blocks*
 - The *flattening transform* essentially combines a longer chain of branches (a->b->c->d) into a looping state machine (a->{b,c,d}), keeping the original order but preventing static analysis from discovering it by computing the next block during program execution
 - It is *allowed* to do anything imaginable as long as the input/output equivalence holds, e.g. the state machine can be arbitrarily complex