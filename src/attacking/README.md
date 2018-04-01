[cff](../cff) -- [analysis](../analysis) -- [attacking](../attacking) -- [attacking](../improving) -- [attacking](../tooling)

# attacks

## symbolic execution

Execute the procedure symbolically and collect operations on _useful_ data.

Data flow analysis can make this more effective.

## reverse engineering the obfuscation algorithm

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

## identifying flattened code

An interesting aspect of code obfuscation is how easily one, with a trained eye, can judge wether the assembly has been obfuscated or not. Some obfuscation techniques can be quite hard to identify, e.g. opaque predicates, dummy blocks and subsitution. Control flow flattening, however, is often very noticable as the resulting control flow graph has a distinguishable structure.

This weakness leads to attacks such as **code lifting** whereby an attacker treats the obfuscated code as a black box. It is possible to modify the code that calls into the black box, as well as extracting it into their own program. Thus, control flow flattening shouldn't be used in order to obfuscate stateless functions, e.g. AES. One could argue that it is possible to introduce global state in the originally stateless (pure) functions and thereby protect against code lifting. This is certainly viable and is used, in the form of **opaque predicates**, to improve control flow flattening.

## using clang to optimize modules

 - piggyback on the compiler:  loop `asm to ir -> recompile w/optimization -> asm to ir`