# control flow flattening

## prelude

Why do we obfuscate code? Is it good? Is it bad? Is it reasonable to obfuscate when SGX and ARM TrustZone exists, if so why?

What does this project focus on? How is it implemented? What are known weaknesses/attacks? Possible improvements?

Layout of this document.

 - section 1: [what is cff?](idea)
 - section 2: [an intro to obfuscation](obfuscation)
 - section 3: [implementation](algorithm)
 - section 4: [cff +](strengths)
 - section 5: [cff -](weaknesses)
 - section 6: [attacking cff](attacks)
 - section 7: [improving cff](improving)
 - section 8: [available tools](tools)
 - section 10: [summary](summary)
 - section 11: [links](links)

## idea

Obfuscate control flow by replacing the function body with a state machine (moore) loop that dispatches the basic blocks. This dispatcher is _very_ often implemented as a switch in high-level code.

## obfuscation

Security through obscurity?

Buying time?

Track record of obfuscation techniques?

## algorithm
A flattening transform implementation could, in the spirit of go, look something like this:
```go
/* a simple flattening algorithm */
func Flatten(f: ir.Function)
    blocks       := f.BasicBlocks()
    switch_var   := ir.Var(types.Int32)
    switch_instr := ir.Switch(switch_var)
    loop_instr   := ir.While(ir.Bool.True)
    
    loop_instr.Add(switch_instr)
    
    /* map each top level block to its own case label */
    for i, block := range(blocks) {  
        /* later on we want to sprinkle some magic on this computation */
        next_block_id   := i + 1
        next_block_expr := switch_var.Store(next_block_id)

        block.Add(next_block_expr)
        block.Add(switch_instr.Break())
        switch_instr.Case(i, block)
    }
    
    f.BasicBlocks().Empty()
    f.Add(switch_var.Decl(0))
    f.Add(loop_instr)
}
```

### example (code)
Here is a very simple example (with some sketchy x86 assembly).
```go
/* original function, { ... } to force flattening  */
func Magic(x: int) int
{
                          // mov eax, x
    { x = x + 1; }        // add eax, 1
    { x = x % 3; }        // mod eax, 3
    { x = x * 2; }        // mul eax, 2
    { return x;  }        // ret
}

/* transforms into this monstrosity */
func Magic(x: int) int
{                       
    block := 0;           // mov block, 0
    while (true) {        // label loop_0_start
        switch (block) {  // label loop_0_switch_0_start
                          //     # jump to case impl.
        case 0:           // label loop_0_switch_0_case_0
            x = x + 1;    //     mov eax, x
                          //     add eax, 1
            block = 1;    //     mov block, 1
            break;        //     jmp loop_0_switch_0_end
        case 1:           // label loop_0_switch_0_case_1
            x = x % 3;    //     mod eax, 3
            block = 2;    //     mov block, 3
            break;        //     jmp loop_0_switch_0_end
        case 2:           // label loop_0_switch_0_case_2
            x = x * 2;    //     mul eax, 2
            block = 3;    //     mov block, 3
            break;        //     jmp loop_0_switch_0_end
        case 3:           // label loop_0_switch_0_case_3
            return x;     //     ret
        }
                          // label loop_0_switch_0_end
                          //     jmp loop_0_start
    }
}
```

### example (graph)
The control flow graphs:

![img 1]()
![img 2]()

## strengths

 - with sophisticated dispatcher constructions the obfuscated control flow is _very_ different from the original
 - possibility to achieve decent protection against dynamic analysis (e.g. using opaque prngs)
 - simple base idea/algorithm that is easy to extend and improve
 - obfuscation improves with other obfuscation techniques: inlining, substitution

## weaknesses

 - very poor cache performance by design (lots of branches)
 - depending on switch impl. and no. of cases it could consume a big chunk of the jump prediction table
 - almost guaranteed 100% branch mispredict in dispatcher (which also results in _very_ poor cache perf.)
 - algorithm used to flatten is easy to undo if known (write optimization/deobfuscate pass for each obfuscation step)
 - cannot _guarantee_ protection against dynamic analysis (branches, memory access) on its own
 - resulting graph is _obviously_ obfuscated and easy to identify --> code can be lifted/hoisted (depends on inlining/fusing)
 
## attacks

 - symbolic execution + analysis of switch var
 - code lifting
 - piggyback on the compiler:  loop `asm to ir -> recompile w/optimization -> asm to ir`

## improving

 - scramble case order
 - split (large) basic blocks
 - duplicate blocks
 - dummy (bogus) blocks
 - opaque predicates
 - dispatcher obfuscation
 - sub-level dispatchers

## tools

## results

## summary


## links

 - [Obfuscating C++ programs via control flow flattening, T László and A Kiss, 2009](http://ac.inf.elte.hu/Vol_030_2009/003.pdf)
