# control flow flattening

## idea

Obfuscate control flow by introducing a loop and state machine (moore) that branches to the next basic block.

## obfuscation

## algorithm

### example (code)
Here is a very simple example (with some sketchy assembly code).
```go
/* original function: no jumps,  */
func Magic(x: int) int
{
                          // mov eax, x
    x = x + 1;            // add eax, 1
    x = x % 3;            // mod eax, 3
    x = x * 2;            // mul eax, 2
                          // mov ??, eax
    return x;             // ret
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
                          //     mov ??, eax
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

## attacks

## hardening

## tools

## results

## summary
