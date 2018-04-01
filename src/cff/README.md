[<~ previous](../) -- [next ~>](../algorithm)

# control flow flattening

Control flow flattening is an obfuscation technique which aims to prevent identification of the control flow by means of static analysis. The obfuscation is based on two components: a *looped switch structure* and a *flattening transformation*. Basically, the flattening transformation rewrites the original control flow by introducing a looped switch to branch between (previously) connected basic blocks:

```
program_in -->       CFF magic       --> program_out

A->B->C->D --> switch + loop rewrite --> X->{A,B,C,D}->X
```

Compare the two following graphs

![original cfg](../bin/intro/abcd.svg)

the cfg of (1) is given by the definition itself, while the cfg of (2) cannot be identified without inspecting the switch machine itself, i.e. how `X` selects the next block. This is the core obfuscation property (obfuscation relies on switch variable) and must therefore be as protected as it can.

But before we head on we should start with a few definitions and observations:

> (D1) Source code is compiled into logical **basic blocks**: units of code defined by the original programming language constructions (if, while, { }, for, etc..)

> (D2) The **control flow graph** describes all the possible execution paths of a procedure or program: one can view the graph nodes as *basic blocks* and graph edges as branches between the blocks (jumps, calls).

> (D3) **Control flow flattening** transforms a function *f* into *F*, with respect to some constraints:
>
>  - same input, same output: *f(x) = F(x)*
>  - stable ordering: *F* contains basic blocks of *f* in original order and executes them in the original order

> (D4) **Static analysis** of a program binary tries to identify *basic blocks* and the edges between them and so uncover the *control flow graph*.

(O1) The analysis in (D4) is not a trivial since the *basic block* is only a compiler/assembler concept and not present in the assembled binary. It is simpler to identify *basic blocks* by their branches than by their content, i.e. mark entry & exit points in the memory and use them to group instructions into blocks.


(O2) Based on (O1) branches convey useful of information about the control flow. So if it is possible to hide the target address of a branch instruction then the analyser can only create an exit point (and not a corresponding entry point somewhere else).

(O3) Since (D3) doesn't prohibit additional CFG nodes it is possible to introduce new blocks into the control flow graph that aren't present in the original. This is essentially what happends when the loop and switch machine is constructed: `loop_start` → `switch(X)` → `{A,B,C,D}` → `loop_end` → `loop_start`.
