# control flow flattening

## prelude

*Why do we obfuscate code? Is it good? Is it bad? Is it reasonable to obfuscate when SGX and ARM TrustZone exists, if so why? What is control flow flattening? Where does it fit in? How is it implemented? What are known weaknesses/attacks? Possible improvements?* 

These are some of the underlying questions and thoughts this article aims to answer and discuss. As with many other things, there isn't a 100%-always-guaranteed-perfectly-correct-forever-and-ever answer. Obfuscation as a solution, however, is often frowned upon and it is hard to argue that it is implemented for the benefit of the consumer/end user. Instead, obfuscation is one of the tools business can use to protect their intellectual property (IP), and, depending on the implementation, may even result in a worse experience for the consumer/end user. While the specific reasonings why obfuscation is used are unknown to me, the general principle is clear: *to protect the product in a hostile environment*. 

 > So what is this hostile environment, you ask? It is your phone, your computer, the wireless network in your home, the wonderful thing we call the internet, the servers that power your services. Generally, it is all the environments which the company does not fully control. One example are games running on your phone, console or computer. Big game publishers are using varying forms of digital rights management (DRM) software to protect their game (IP) against crackers and piracy. 
 
To make some discussions easier and more informative these environments will be separated into *phone*, *pc* and *servers*. A short overview of protective measures and known attacks will be presented for each one of these environments. 

Layout of this document.

 - section 1: [what is cff?](#idea)
 - section 2: [an intro to obfuscation](#obfuscation)
 - section 3: [implementation](#algorithm)
 - section 4: [cff +](#strengths)
 - section 5: [cff -](#weaknesses)
 - section 6: [attacking cff](#attacks)
 - section 7: [improving cff](#improving)
 - section 8: [available tools](#tools)
 - section 10: [summary](#summary)
 - section 11: [links](#links)

## idea

Apply a flattening transform on the control flow that hides the original flow while keeping a strict equivalence in terms of input/output. The core assumptions and observations can be expressed as:

 - The *source code* is compiled into *machine instructions* by the compiler
 - Machine *instructions* can be grouped into *blocks* by jumps & entry points
 - These *blocks* are chained together to form *procedures*
 - A *program* consists of one or more *procedures*
 - The control flow graph is a graph which describes all possible execution paths of a particular *program*, *procedure* or *block*
 - Static analysis of the control flow graph is constructed by finding branches (implicit & explicit) between *blocks*
 - The *flattening transform* essentially combines a longer chain of branches (a->b->c->d) into a looping state machine (a->{b,c,d}), keeping the original order but preventing static analysis from discovering it by computing the next block during program execution
 - It is *allowed* to do anything imaginable as long as the input/output equivalence holds, e.g. the state machine can be arbitrarily complex

## obfuscation

Obfuscation is often regarded as a subpar solution to a difficult problem. While its reputation isn't necessarily undue it is important to recognise that obfuscation is a means to an end, whatever that is, with both poor and solid implementations.

As with security, obfuscation is outrageously hard to get right and simple mistakes and oversights can be devastating. Once the product is shipped, that's it - it's out there for the world to see and (ab)use.

Track record of obfuscation techniques?

### on your smartphone

 - effect of Intel SGX, AMD ??, ARM TrustZone
 - vendor-controlled system (apple fairplay, google ??, microsoft ??)
 - exploitable --> rootable --> hostile
 - give your program to apple & trust them

### on your computer

 - effect of Intel SGX, AMD ??, ARM TrustZone
 - every man for himself
 - 3rd party protection: Denuvo, VMProtect
 - give your program to end user & trust them

### on someone else's server

 - effect of Intel SGX, AMD ??, ARM TrustZone
 - give your program to server owner & trust them

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

        /* skip block=next and break if block terminates function */
        if !block.AllCodePathsReturn() {
          block.Add(next_block_expr)
          block.Add(switch_instr.Break())
        }
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
The assembly of the flattened function is indeed more complicated and does more work compared to the original. The function output, however, is equivalent to the original.

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
An obvious weakness of control flow flattening is the **performance** penalty caused by **cache locality** and other memory-related problems. While many compilers try to optimize the branches of a switch construction the flattened graph is by design the worst possible case: a shitload of unique cases.

Most compilers will optimize and generate a binary tree comparison procedure, with O(log N) complexity, for finding the correct case and this helps tremendously compared to standard linear probing, which is O(N). In fact most modern compilers will generate a combination of the two. This limits the penalty incurred by increasing the number of cases, which is required by some hardening techniques.

Each branch in the dispatcher takes up a slot in the branch predictor (of the CPU), which is guaranteed to be useless since the branch predictor _cannot_ predict the correct branch, unless we have a constant value obfuscated as an opaque predicate. Thus the generated branch will always have a mispredict overhead associated with it.

 - algorithm used to flatten is easy to undo if known (write optimization/deobfuscate pass for each obfuscation step)
 - cannot _guarantee_ protection against dynamic analysis (branches, memory access) on its own
 - resulting graph is _obviously_ obfuscated and easy to identify --> code can be lifted/hoisted (depends on inlining/fusing)
 
## attacks

### symbolic execution

Execute the procedure symbolically and collect operations on _useful_ data.

Data flow analysis can make this more effective.

### reverse engineering the obfuscation algorithm

extremely simplified example on how to reverse the algorithm:
```go
/* unflattens an IR representation of the flattened function */
func Unflatten(function_blocks ir.BasicBlock[]) ir.Function {
    /* this depends on calling convention */
    fn_params := ir.FindParams(function_blocks)
    original  := ir.Function(fn_params)
  
    prelude    := function_blocks[0]
    dispatcher := prelude.Find(ir.Switch.type)
    block_var  := dispatcher.Var
  
    prelude.Remove(dispatcher)
    prelude.Remove(block_var)
    original.Add(prelude)
  
    for _, bb := range(function_blocks[1:]) {
        if !bb.AllCodePathsReturn() {
            bb.Remove(-2)
        }
    
        original.Add(bb)
    }
    
    return original
}
```

### identifying flattened code

An interesting aspect of code obfuscation is how easily one, with a trained eye, can judge wether the assembly has been obfuscated or not. Some obfuscation techniques can be quite hard to identify, e.g. opaque predicates, dummy blocks and subsitution. Control flow flattening, however, is often very noticable as the resulting control flow graph has a distinguishable structure.

This weakness leads to attacks such as **code lifting** whereby an attacker treats the obfuscated code as a black box. It is possible to modify the code that calls into the black box, as well as extracting it into their own program. Thus, control flow flattening shouldn't be used in order to obfuscate stateless functions, e.g. AES. One could argue that it is possible to introduce global state in the originally stateless (pure) functions and thereby protect against code lifting. This is certainly viable and is used, in the form of **opaque predicates**, to improve control flow flattening.

### using clang to optimize modules

 - piggyback on the compiler:  loop `asm to ir -> recompile w/optimization -> asm to ir`

## improving

This section will try to describe improvements upon control flow flattening in a general sense and in relation to the toy implementation.

The most important aspect of any obfuscation technique is to generate **non-deterministic** output. The program _result_ should very much still be deterministic but the generated code mustn't be, or you risk an attacker cracking _all_ your programs. A simple solution is to sprinkle some **randomness** whenever possible. Some candidates are *case ordering*, *block layout* and *successor computation*.

 - scramble case order
 - split (large) basic blocks
 - duplicate blocks
 - dummy (bogus) blocks
 - opaque predicates
 - dispatcher obfuscation
 - sub-level dispatchers
 
### from the environment

A smartphone paltform can improve on control flow obfuscation by recompiling/retransforming the application sent to each user, such that each user ends up with a somewhat different binary.
 
### opaque predicates

As discussed previously in the section regarding **code lifting**, this obfuscation method can be used to introduce (useless) state obfuscation which is hard to both identify and undo. Consider this example:

```c
if (true) {
    return 0;
} else {         
    return 1;
}
```

The first branch will always be taken but a standard control flow graph doesn't show that. An analyser must first inspect the branch statement and try to simplify it before the control flow graph can be reduced. Opaque predicates try to make this analysis as difficult as possible.

Some implementation methods use arithmetic (in)equalities to generate the predicate but these (in)equalities are difficult to construct and, once known, trivial to identify and simplify. From this problem ? to have the predicate masquerade as legit instructions. 

## tools

### [OLLVM](https://github.com/obfuscator-llvm/obfuscator/)

## results

### benchmarks

 - execution time
 - throughput
 - latency?
 - l2 cache miss?

### analysis

 - dispatcher tree breadth
 - dispatcher tree depth

## summary

In the end, (software) obfuscation is used simply because there isn't a better alternative available, yet.


## links

 - [Obfuscating C++ programs via control flow flattening, T László and A Kiss, 2009](http://ac.inf.elte.hu/Vol_030_2009/003.pdf)
