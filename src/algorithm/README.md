[<~ previous](../obfuscation)
[next ~>](../analysis)

# 3 - implementation

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

## example (code)
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

## example (graph)
The control flow graphs:

![img 1]()
![img 2]()